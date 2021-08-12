#include "window_context.hpp"

//constructor
WindowContext::WindowContext(){
    if (!glfwInit()) printf("GLFW: failed to init");
    //4.1 code
    /*
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    */
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(WIDTH, HEIGHT, "Mandelbrot Set", NULL, NULL);
    if (!window) {
        printf("GLFW: failed to create the window");
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    //initialize glew
    if (glewInit() != 0) printf("GLEW: failed to init");
}

void WindowContext::loop(void (*cb)()){
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        cb();
        //this is the place where actual drawing takes place
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
void WindowContext::close(){
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
    
    glfwSetWindowShouldClose(window, 1);
    glfwDestroyWindow(window);
    glfwTerminate();
}
void WindowContext::onError(void (*cb)(int, const char*)){
    glfwSetErrorCallback(cb);
}
void WindowContext::onKey(void (*cb)(GLFWwindow*, int, int, int, int)){
    glfwSetKeyCallback(window, cb);
}
void WindowContext::onMouseMove(void (*cb)(GLFWwindow*, double, double)){
    glfwSetCursorPosCallback(window, cb);
}
void WindowContext::onMouseButton(void (*cb)(GLFWwindow*, int, int, int)){
    glfwSetMouseButtonCallback(window, cb);
}
void WindowContext::onScroll(void (*cb)(GLFWwindow*, double, double)){
    glfwSetScrollCallback(window, cb);
}
void WindowContext::onResize(void (*cb)(GLFWwindow*, int, int)){
    glfwSetWindowSizeCallback(window, cb);
}





//utility used for shader loading
char *readFileVarLength(FILE *fp){
    printf("reading a file\n");
    char *line = NULL;
    size_t linebufflen = 0;
    ssize_t linelen;
    
    int bufflen = 512;
    int currentPosition = 0; 
    char *buff = (char*)malloc(bufflen);
    
    while ((linelen = getline(&line, &linebufflen, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", linelen);
        //printf("allocated buffer of length %zu:\n", linebufflen);
        //printf("got the line: (%s)\n",line);
        
        if(currentPosition+linelen+2 > bufflen){
            bufflen *= 2;
            buff = (char*)realloc(buff,bufflen);
        }
        
        strncpy(&buff[currentPosition],line,linelen);
        currentPosition += linelen;
        //printf("current position: %d\n",currentPosition);
    }

    free(line);
    return buff;
}

char* readFileNameVarLength(const char* fname){
    FILE *fp;
    if ((fp = fopen(fname, "r")) == NULL) {
        fprintf(stderr, "Error opening the file %s\n", fname);
        exit(1);
    }
    return readFileVarLength(fp);
}

void WindowContext::compileShaders(const char* vpath,const char* fpath){
    char* vertexSource = readFileNameVarLength(vpath);
    char* fragmentSource = readFileNameVarLength(fpath);
    
    //v and f shader ids
    unsigned int vertex, fragment;
    //shader compilation success flag
    int success;
    //log in case something goes wrong
    char infoLog[1024];
    
    //create a shader, and refer to it with an uint from now on
    vertex = glCreateShader(GL_VERTEX_SHADER);
    //add source to the shader number
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    //see if it was successful or not
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        //in case of an error
        //retrieving the info log
        glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
        //and display
        printf("ERROR::SHADER_COMPILATION_ERROR of type: VERTEX\n%s\n ------------------------------------------------------ -- ", infoLog);
    }

    //the same as the vertex shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 1024, NULL, infoLog);
        printf("ERROR::SHADER_COMPILATION_ERROR of type: FRAGMENT\n%s\n ------------------------------------------------------ -- ", infoLog);
    }
    
    //generating a new shader program, and storing its id in shaderProgram as a uint
    shaderProgram = glCreateProgram();
    //attach vertex and frag shaders to shaderProgram
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    //link it
    glLinkProgram(shaderProgram);
    //delete v and f, now that they're linked
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    //switch the context to shaderProgram
    glUseProgram(shaderProgram);
}
void WindowContext::setVertices(float* vertices, size_t size){
    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}
GLint WindowContext::getUniformPointer(const char* name){
    return glGetUniformLocation(shaderProgram, name);
}


void WindowContext::setUniform1f(const char* name, float a){
    glUniform1f(glGetUniformLocation(shaderProgram, name),a);
}
void WindowContext::setUniform2f(const char* name, float a, float b){
    glUniform2f(glGetUniformLocation(shaderProgram, name),a,b);
}
void WindowContext::setUniform3f(const char* name, float a, float b, float c){
    glUniform3f(glGetUniformLocation(shaderProgram, name),a,b,c);
}
void WindowContext::setUniform4f(const char* name, float a, float b, float c, float d){
    glUniform4f(glGetUniformLocation(shaderProgram, name),a,b,c,d);
}
void WindowContext::setUniform1i(const char* name, int a){
    glUniform1i(glGetUniformLocation(shaderProgram, name),a);
}
void WindowContext::setUniform2i(const char* name, int a, int b){
    glUniform2i(glGetUniformLocation(shaderProgram, name),a,b);
}
void WindowContext::setUniform3i(const char* name, int a, int b, int c){
    glUniform3i(glGetUniformLocation(shaderProgram, name),a,b,c);
}
void WindowContext::setUniform4i(const char* name, int a, int b, int c, int d){
    glUniform4i(glGetUniformLocation(shaderProgram, name),a,b,c,d);
}


void WindowContext::setViewport(int x, int y, int width, int height){
    glViewport(0, 0, width, height);
}

