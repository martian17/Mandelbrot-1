#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//class definition
class WindowContext {
    public:
    WindowContext();//constructor
    void loop(void (*cb)());
    void close();
    //callbacks
    void onError      (void (*cb)(int, const char*));
    void onKey        (void (*cb)(GLFWwindow*, int, int, int, int));
    void onMouseMove  (void (*cb)(GLFWwindow*, double, double));
    void onMouseButton(void (*cb)(GLFWwindow*, int, int, int));
    void onScroll     (void (*cb)(GLFWwindow*, double, double));
    void onResize     (void (*cb)(GLFWwindow*, int, int));
    //shader related
    void compileShaders(const char* vpath,const char* fpath);
    void setVertices(float* vertices, size_t size);
    GLint getUniformPointer(const char* name);
    //uniforms
    void setUniform1f(const char* name, float a);
    void setUniform2f(const char* name, float a, float b);
    void setUniform3f(const char* name, float a, float b, float c);
    void setUniform4f(const char* name, float a, float b, float c, float d);
    void setUniform1i(const char* name, int a);
    void setUniform2i(const char* name, int a, int b);
    void setUniform3i(const char* name, int a, int b, int c);
    void setUniform4i(const char* name, int a, int b, int c, int d);
    
    void setViewport(int x, int y, int width, int height);
    
    //variables
    int WIDTH  = 800;
    int HEIGHT = 600;
    bool dragging = false;
    double oldX, oldY;
    unsigned int shaderProgram;
    GLFWwindow* window;
    unsigned int vao, vbo;
};

//split the funcdef to cpp just for the learning sake