#include <stdio.h>


#include <kit/app/app.h>
#include <kit/view/view.h>
#include <kit/window/window.h>

#include <extern/glad/glad.h>

#include <time.h>
#include <unistd.h>

#include "stb_image.h"

WindowClass windowClass;

extern const unsigned char resources_shaders_shader_frag[];
extern uint32_t resources_shaders_shader_frag_size;

extern const unsigned char resources_shaders_shader_vert[];
extern uint32_t resources_shaders_shader_vert_size;

unsigned int shaderProgram;

unsigned int modelUniform;
float model[4][4];

unsigned int VBO, VAO, EBO;
unsigned int texture;
unsigned int texture2;

void MyRenderCallback(void *window)
{
    printf("Render Callback\n");

    // Time the rendering process
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    
    int divisionsX = 101;
    int divisionsY = 101;
    int width = ((Window*)window)->size.width;
    int height = ((Window*)window)->size.height;

    bool bUseTexture = true;

    for (int y = 0; y < divisionsY; y++)
    {
        for (int x = 0; x < divisionsX; x++)
        {
            // bind Texture
            if (bUseTexture) {
                glBindTexture(GL_TEXTURE_2D, texture2);
            } else {
                glBindTexture(GL_TEXTURE_2D, texture);
            }
            
            bUseTexture = !bUseTexture;

            // draw our first triangle
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
            //glDrawArrays(GL_TRIANGLES, 0, 6);

            // Create orthographic projection (top-left quarter)
            // Identity matrix
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (i == j) {
                        model[i][j] = 1.0f;
                    } else {
                        model[i][j] = 0.0f;
                    }
                }
            }

            // Apply scaling
            model[0][0] = 1.0f / divisionsX;  // Scaling in X
            model[1][1] = 1.0f / divisionsY;  // Scaling in Y

            // Apply translation
            model[0][3] = -1.0 + (2.0f / divisionsX) * ((int)x + 0.5f); // Translation in X
            model[1][3] = -1.0 + (2.0f / divisionsY) * ((int)y + 0.5f); // Translation in Y

            glUniformMatrix4fv(modelUniform, 1, GL_FALSE, (const float*)model);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        }
    }


    

    clock_gettime(CLOCK_MONOTONIC, &end);
    
    long elapsedMicros = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    printf("Render time: %ld microseconds\n", elapsedMicros);
}

void MyWindowResizeCallback(void *window, Size newSize)
{
    //printf("Window resized to %.1f x %.1f\n", newSize.width, newSize.height);

}

void AppLaunched(void *app)
{
    printf("App Launched\n");

    Window *window = CreateWindow(&windowClass, "My Window", 800, 600);

    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    printf("OpenGL version: %d.%d\n", major, minor);

    const char* vertShader = (const char *)resources_shaders_shader_vert;
    const char* fragShader = (const char *)resources_shaders_shader_frag;

    printf("Loaded shader vert: \n%s\n", vertShader);
    printf("Loaded shader frag: \n%s\n", fragShader);


    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertShader, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }
    else
    {
        printf("Vertex Shader Compiled\n");
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragShader, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }
    else
    {
        printf("Fragment Shader Compiled\n");
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    modelUniform = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelUniform, 1, GL_FALSE, (const float*)model);

    float vertices[] = {
        // positions          // colors           // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

     glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

      // load and create a texture 
    // -------------------------
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load("build/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    data = stbi_load("build/metal.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture\n");
    }
    stbi_image_free(data);

    printf("Shader Program: %d\n", shaderProgram);
    
    View *view = CreateView(GetViewClassByName("org.nanokit.dock"));
    window->rootView = view;
}

int main()
{   

    windowClass = (WindowClass) {
        .name = "MyWindowClass",
        .dataSize = 0,
        .resizeCallback = MyWindowResizeCallback,
        .renderCallback = MyRenderCallback
    };

    RegisterWindowClass(&windowClass);

    App app = 
    {
        .name = "MyApp",
        .description = "My App Description",
        .launchedCallback = AppLaunched
    };

    return RunApp(&app);
}