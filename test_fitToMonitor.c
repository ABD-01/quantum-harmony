/**
 * @file    test_fitToWindow.c
 * @author  github.com/ABD-01
 * @date    6 May 2025
 * @brief   This file was created to test issue #4880 in raylib.
 *          Link: https://github.com/raysan5/raylib/issues/4880
 *
 * @license MIT
 *
 */

 #include <windows.h>
 #include <stdio.h>
 #include <stdint.h>
 #include <stdbool.h>
 
 #include <glad/gl.h>
 #define GLFW_INCLUDE_NONE
 #include <GLFW/glfw3.h>
 
 
 uint32_t display_width;
 uint32_t display_height;
 uint32_t screen_width;
 uint32_t screen_height;
 uint32_t render_width;
 uint32_t render_height;
 uint32_t currentFbo_width;
 uint32_t currentFbo_height;
 
 // -----------------------------------
 //   AI Generated Code Start 
 // -----------------------------------
 
 // Vertex Shader Source
 const char *vertexShaderSource = "#version 330 core\n"
     "layout (location = 0) in vec3 aPos;\n"
     "void main()\n"
     "{\n"
     "   gl_Position = vec4(aPos, 1.0);\n"
     "}\0";
 
 // Fragment Shader Source
 const char *fragmentShaderSource = "#version 330 core\n"
     "out vec4 FragColor;\n"
     "void main()\n"
     "{\n"
     "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
     "}\0";
 
 void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
     glViewport(0, 0, width, height);
 }
 
 void processInput(GLFWwindow *window) {
     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
         glfwSetWindowShouldClose(window, 1);
 }
 // -----------------------------------
 //   AI Generated Code End
 // -----------------------------------
 
 
 static void ErrorCallback(int error, const char *description)
 {
     fprintf(stderr, "GLFW: Error: %i Description: %s", error, description);
 }
 
 static void SetDimensionsFromMonitor(GLFWmonitor *monitor)
 {
   const GLFWvidmode *mode = glfwGetVideoMode(monitor);
 
   // Default display resolution to that of the current mode
   display_width = mode->width;
   display_height = mode->height;
 
   // Set screen width/height to the display width/height if they are 0
   if (screen_width == 0) screen_width = display_width;
   if (screen_height == 0) screen_height = display_height;
 
   printf("SetDimensionsFromMonitor: display_width, display_height: (%i, %i)\n", display_width, display_height);
 }
 
 int main(void)
 {
     glfwSetErrorCallback(ErrorCallback);
 
     int result = glfwInit();
     if (result == GLFW_FALSE) { fprintf(stderr, "GLFW: Failed to initialize GLFW\n"); return -1; }
 
     glfwDefaultWindowHints();
     glfwWindowHint(GLFW_AUTO_ICONIFY, 0);
     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
     screen_width = 0;
     screen_height = 0;
 
   // From raylib\src\platforms\rcore_desktop_glfw.c line: 1500
     bool requestWindowedFullscreen = (screen_height == 0) && (screen_width == 0);
 
     GLFWmonitor *monitor = NULL;
     monitor = glfwGetPrimaryMonitor();
     
   // From raylib\src\platforms\rcore_desktop_glfw.c line: 1504
     int creationWidth = screen_width != 0 ? screen_width : 1;
     int creationHeight = screen_height != 0 ? screen_height : 1;
 
     GLFWwindow *window = glfwCreateWindow(creationWidth, creationHeight, "Fit To Monitot Test ", NULL, NULL);
     if (!window) {
         glfwTerminate();
         printf("GLFW: Failed to initialize Window\n");
         return -1;
     }
 
   // From raylib\src\platforms\rcore_desktop_glfw.c line: 1521
     SetDimensionsFromMonitor(monitor);
     if (requestWindowedFullscreen) glfwSetWindowSize(window, screen_width, screen_height);
 
 
   // From raylib\src\platforms\rcore_desktop_glfw.c line: 1545
     glfwMakeContextCurrent(window);
     result = glfwGetError(NULL);
 
     if ((result != GLFW_NO_WINDOW_CONTEXT) && (result != GLFW_PLATFORM_ERROR))
     {
 
   // From raylib\src\platforms\rcore_desktop_glfw.c line: 1565
         int fbWidth = screen_width;
         int fbHeight = screen_height;
 
   // From raylib\src\platforms\rcore_desktop_glfw.c line: 1583
         render_width = fbWidth;
         render_height = fbHeight;
         currentFbo_width = fbWidth;
         currentFbo_height = fbHeight;
 
         printf("DISPLAY: Device initialized successfully\n");
         printf("    > Display size: %i x %i\n", display_width, display_height);
         printf("    > Screen size:  %i x %i\n", screen_width, screen_height);
         printf("    > Render size:  %i x %i\n", render_width, render_height);
         // printf("    > Viewport offsets: %i, %i\n", CORE.Window.renderOffset.x, CORE.Window.renderOffset.y);
     }
     else
     {
         printf("PLATFORM: Failed to initialize graphics device\n");
         return -1;
     }
 
     int monitorX = 0;
     int monitorY = 0;
     int monitorWidth = 0;
     int monitorHeight = 0;
     glfwGetMonitorWorkarea(monitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);
 
     printf("Monitor: %i, %i, %i, %i\n", monitorX, monitorY, monitorWidth, monitorHeight);
 
     // Here render_width/height should be used instead of screen_width/height to center the window correctly when the high dpi flag is enabled.
     int posX = monitorX + (monitorWidth - (int)render_width)/2;
     int posY = monitorY + (monitorHeight - (int)render_height)/2;
     if (posX < monitorX) posX = monitorX;
     if (posY < monitorY) posY = monitorY;
     // SetWindowPosition(posX, posY);
 
   // From raylib\src\platforms\rcore_desktop_glfw.c line: 608
     glfwSetWindowPos(window, posX, posY);
 
     // Update CORE.Window.position here so it is correct from the start
     // CORE.Window.position.x = posX;
     // CORE.Window.position.y = posY;
     printf("Core.Window.position: %i, %i\n", posX, posY);
 
     if (gladLoadGL((GLADloadfunc)glfwGetProcAddress) == 0) 
         printf("GLAD: Cannot load OpenGL extensions\n");
     else 
         printf("GLAD: OpenGL extensions loaded successfully\n");
 
 
     glfwSwapInterval(1);
 
     printf("PLATFORM: DESKTOP (GLFW - Win32): Initialized successfully\n");
 
     glViewport(0, 0, render_width, render_height);
 
 
     // It is so sad that we cannot use OpenGl apis like we used to in 1.1
     // I will use AI to generate shaders in OpenGL 3.3.0
 
     // -----------------------------------
     //   AI Generated Code Start 
     // -----------------------------------
 
     // Compile Vertex Shader
     unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
     glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
     glCompileShader(vertexShader);
     int success;
     char infoLog[512];
     glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
     if (!success) {
         glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
         printf("Vertex Shader Compilation Failed: %s\n", infoLog);
     }
 
     // Compile Fragment Shader
     unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
     glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
     glCompileShader(fragmentShader);
     glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
     if (!success) {
         glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
         printf("Fragment Shader Compilation Failed: %s\n", infoLog);
     }
 
     // Link Shader Program
     unsigned int shaderProgram = glCreateProgram();
     glAttachShader(shaderProgram, vertexShader);
     glAttachShader(shaderProgram, fragmentShader);
     glLinkProgram(shaderProgram);
     glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
     if (!success) {
         glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
         printf("Shader Program Linking Failed: %s\n", infoLog);
     }
     glDeleteShader(vertexShader);
     glDeleteShader(fragmentShader);
 
     // Rectangle vertices (two triangles)
     // float vertices[] = {
         // -0.5f, -0.5f, 0.0f, // Bottom-left
          // 0.5f, -0.5f, 0.0f, // Bottom-right
          // 0.5f,  0.5f, 0.0f, // Top-right
         // -0.5f,  0.5f, 0.0f  // Top-left
     // };
     float rect_width = 50.0f;
     float rect_height = 50.0f;
     float vertices[] = {
         -1.0f,  1.0f, 0.0f,       // Top-left (0,0)
         -1.0f + 2.0f * rect_width / screen_width,  1.0f, 0.0f,   // Top-right
         -1.0f + 2.0f * rect_width / screen_width,  1.0f - 2.0f * rect_height / screen_height, 0.0f, // Bottom-right
         -1.0f,  1.0f - 2.0f * rect_height / screen_height, 0.0f     // Bottom-left
     };
     unsigned int indices[] = {
         0, 1, 2, // First triangle
         2, 3, 0  // Second triangle
     };
 
     // Set up VAO, VBO, EBO
     unsigned int VAO, VBO, EBO;
     glGenVertexArrays(1, &VAO);
     glGenBuffers(1, &VBO);
     glGenBuffers(1, &EBO);
 
     glBindVertexArray(VAO);
 
     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
 
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
     glEnableVertexAttribArray(0);
 
     glBindBuffer(GL_ARRAY_BUFFER, 0);
     glBindVertexArray(0);
 
 
     // -----------------------------------
     //   AI Generated Code end
     // -----------------------------------
 
     while (!glfwWindowShouldClose(window)) {
         processInput(window);
 
         // Clear screen
         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT);
 
         // Draw rectangle
         glUseProgram(shaderProgram);
         glBindVertexArray(VAO);
         glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
 
         glfwSwapBuffers(window);
         glfwPollEvents();
     }
 
     // Clean up
     glDeleteVertexArrays(1, &VAO);
     glDeleteBuffers(1, &VBO);
     glDeleteBuffers(1, &EBO);
     glDeleteProgram(shaderProgram);
 
     glfwDestroyWindow(window);
     glfwTerminate();
     return 0;
 }