#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define LOG(x) std::cout << x << std::endl;

#ifdef _WIN32 // hides console if on windows
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int WIDTH  = 800;
int HEIGHT = 600;

int itr = 500;
double zoom = 100.0;
double offsetX = 0.0;
double offsetY = 0.0;

bool dragging = false;
double oldX, oldY;

unsigned int shaderProgram;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void window_size_callback(GLFWwindow* window, int width, int height);

int main() {
    if (!glfwInit()) LOG("GLFW: failed to init")
    
    /*
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    */
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Mandelbrot Set", NULL, NULL);
    
    if (!window) {
        LOG("GLFW: failed to create the window")
        glfwTerminate();
    }

    glfwSetErrorCallback([](int e, const char *s) { std::cerr << s << std::endl; });
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    
    glfwMakeContextCurrent(window);
    
    if (glewInit() != 0) LOG("GLEW: failed to init")
    
    float vertices[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
     1.0f,  1.0f,
    
     1.0f,  1.0f,
    -1.0f,  1.0f,
    -1.0f, -1.0f
    };
    
    const char* vertexSource =
	"#version 330 core                                        \n"
    "layout(location = 0) in vec2 pos;                        \n"
    "                                                         \n"
    "void main() {                                            \n"
    "gl_Position = vec4(pos,0.0,1.0);                         \n"
    "}                                                        \n";
    
    const char* fragmentSource =
    "#version 330 core                                        \n"
    "                                                         \n"
    "uniform int itr;                                         \n"
    "uniform float zoom;                                      \n"
    "uniform vec2 screenSize;                                 \n"
    "uniform vec2 offset;                                     \n"
    "                                                         \n"
    "int n = 0;                                               \n"
    "float threshold = 100.0;                                 \n"
    "float flmz(float a){                                     \n"
    "    if(a<0.0){                                           \n"
    "    return 0.0;                                          \n"
    "}else{                                                   \n"
    "    return a;                                            \n"
    "}                                                        \n"
    "}                                                        \n"
    
    "//colors                                                 \n"
    "float red(int j){                                        \n"
    "    return flmz(sin(float(j)/100.0));                    \n"
    "};                                                       \n"
    "float gre(int j){                                        \n"
    "    return flmz(sin(float(j)/100.0+1.0));                \n"
    "};                                                       \n"
    "float blu(int j){                                        \n"
    "    return flmz(sin(float(j)/100.0+2.0));                \n"
    "};                                                       \n"
    "                                                         \n"
    "int mandelbrot(vec2 c) {                               \n"
    "    vec2 z = vec2(0.0,0.0);                                  \n"
    "    for(int i = 0; i < itr; i++){                            \n"
    "        vec2 znew;                                               \n"
    "        znew.x = (z.x * z.x) - (z.y * z.y) + c.x;                \n"
    "        znew.y = (2.0 * z.x * z.y) +c.y;                         \n"
    "        z = znew;                                                \n"
    "        if((z.x * z.x) + (z.y * z.y) > threshold)break;          \n"
    "        n++;                                                     \n"
    "    }                                                        \n"
    "    return n;                                            \n"
    "}                                                        \n"
    "                                                         \n"
    "vec4 map_to_color(int t) {                             \n"
    "    //float r = 9.0 * (1.0 - t) * t * t * t;                   \n"
    "    //float g = 15.0 * (1.0 - t) * (1.0 - t) * t * t;          \n"
    "    //float b = 8.5 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t;   \n"
    "    if(t == itr){                                                         \n"
    "        return vec4(0.0,0.0,0.0,1.0);                                                     \n"
    "    }                                                         \n"
    "    return vec4(red(t),gre(t),blu(t),1.0);                        \n"
    "    //return vec4(r, g, b, 1.0);                               \n"
    "}                                                        \n"
    "                                                         \n"
    "void main() {                                            \n"
    "    vec2 coord = vec2(gl_FragCoord.xy);                      \n"
    "    int t = mandelbrot(((coord - screenSize/2)/zoom) - offset);\n"
    "    if(gl_FragCoord.x < 40){                                 \n"
    "        gl_FragColor = vec4(1.0);                                \n"
    "    }                                                        \n"
    "    gl_FragColor = map_to_color(t);                          \n"
    "}                                                        \n";
    
    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    
    unsigned int vertex, fragment;
    int success;
    char infoLog[1024];
    
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: VERTEX\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: FRAGMENT\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    glUseProgram(shaderProgram);
    
    glUniform2f(glGetUniformLocation(shaderProgram, "screenSize"), (float)WIDTH, (float)HEIGHT);
    glUniform2f(glGetUniformLocation(shaderProgram, "offset"), (float)offsetX, (float)offsetY);
    glUniform1f(glGetUniformLocation(shaderProgram, "zoom"), (float)zoom);
    glUniform1i(glGetUniformLocation(shaderProgram, "itr"), itr);
    
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        LOG("frame");
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
    
    glfwDestroyWindow(window);
    glfwTerminate();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
    
    if (key == GLFW_KEY_KP_0 && action == GLFW_PRESS) {
        itr = 2000;
        zoom = 100.0;
        offsetX = 0.0;
        offsetY = 0.0;
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    offsetX += 20 / zoom;
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
    offsetX -= 20 / zoom;
    else if (key == GLFW_KEY_W && action == GLFW_PRESS)
    offsetY -= 20 / zoom;
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
    offsetY += 20 / zoom;

    if (key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
    zoom *= 2;
    else if (key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
    zoom /= 2;

    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    itr += 50;
    else if (key == GLFW_KEY_E && action == GLFW_PRESS)
    (itr > 100) ? itr -= 50 : itr = 50;
    
    glUniform1i(glGetUniformLocation(shaderProgram, "itr"), itr);
    glUniform1f(glGetUniformLocation(shaderProgram, "zoom"), (float)zoom);
    glUniform2f(glGetUniformLocation(shaderProgram, "offset"), (float)offsetX, (float)offsetY);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &oldX, &oldY);
        dragging = true;
    }else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        dragging = false;
    }
    
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (dragging) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        offsetX += (xpos - oldX) / zoom;
        offsetY += (oldY - ypos) / zoom;
        
        oldX = xpos;
        oldY = ypos;
        
        glUniform2f(glGetUniformLocation(shaderProgram, "offset"), (float)offsetX, (float)offsetY);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset != 0) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        double dx = (xpos - WIDTH / 2) / zoom - offsetX;
        double dy = (HEIGHT - ypos - HEIGHT / 2) / zoom - offsetY;
        offsetX = -dx;
        offsetY = -dy;
        if (yoffset < 0)
        zoom /= 1.2;
        else
        zoom *= 1.2;
        
        dx = (xpos - WIDTH / 2) / zoom;
        dy = (HEIGHT - ypos - HEIGHT / 2) / zoom;
        offsetX += dx;
        offsetY += dy;
        
        glUniform1f(glGetUniformLocation(shaderProgram, "zoom"), (float)zoom);
        glUniform2f(glGetUniformLocation(shaderProgram, "offset"), (float)offsetX, (float)offsetY);
    }
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    WIDTH = width;
    HEIGHT = height;
    glUniform2f(glGetUniformLocation(shaderProgram, "screenSize"), (float)WIDTH, (float)HEIGHT);
}
