#include "window_context.hpp"

//portability
#ifdef _WIN32 // hides console if on windows
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif


//defining callbacks
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void window_size_callback(GLFWwindow* window, int width, int height);

//defining various global variables
int WIDTH  = 800;
int HEIGHT = 600;

int itr = 500;
double zoom = 100.0;
double offsetX = 0.0;
double offsetY = 0.0;

bool dragging = false;
double oldX, oldY;
double xpos, ypos;

//initialize the class
WindowContext ctx;

//main, yes!!!
int main(){
    //setting all the events
    ctx.onError      ([](int e, const char* s) { printf("glfw error(%d): %s\n",e,s); });
    ctx.onKey        (key_callback);
    ctx.onMouseMove  (cursor_position_callback);
    ctx.onMouseButton(mouse_button_callback);
    ctx.onScroll     (scroll_callback);
    ctx.onResize     (window_size_callback);
    
    ctx.compileShaders("./shader.vert","./shader.frag");
    
    //adding vertices to buffer
    float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        
         1.0f,  1.0f,
        -1.0f,  1.0f,
        -1.0f, -1.0f
    };
    ctx.setVertices(vertices,sizeof(vertices));
    
    //uniform locations
    ctx.setUniform2f("screenSize", (float)WIDTH,   (float)HEIGHT);
    ctx.setUniform2f("offset",     (float)offsetX, (float)offsetY);
    ctx.setUniform1f("zoom",       (float)zoom);
    ctx.setUniform1i("itr",        itr);
    
    //main loop where events will be executed
    ctx.loop([]() {
        //printf("z:%f x:%f y:%f\n",zoom,offsetX,offsetY);
        //printf("x:%f y:%f\n",oldX,oldY);
    });
    
    //close everything up
    ctx.close();
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        ctx.close();
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
    
    ctx.setUniform1i("screenSize", itr);
    ctx.setUniform1f("zoom",       (float)zoom);
    ctx.setUniform2f("offset",     (float)offsetX, (float)offsetY);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        oldX = xpos;
        oldY = ypos;
        dragging = true;
    }else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        dragging = false;
}
    
void cursor_position_callback(GLFWwindow* window, double xpos0, double ypos0) {
    //printf("x:%f y:%f\n",xpos,ypos);
    xpos = xpos0;
    ypos = ypos0;
    if (dragging) {
        //printf("dragging\n");
        offsetX += (xpos - oldX) / zoom;
        offsetY += (oldY - ypos) / zoom;
        
        oldX = xpos;
        oldY = ypos;
        
        ctx.setUniform2f("offset",     (float)offsetX, (float)offsetY);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset != 0) {
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
        
        //printf("x:%f y:%f\n",xpos,ypos);
        //printf("z:%f x:%f y:%f\n",zoom,offsetX,offsetY);
        ctx.setUniform1f("zoom",       (float)zoom);
        ctx.setUniform2f("offset",     (float)offsetX, (float)offsetY);
    }
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    ctx.setViewport(0, 0, width, height);
    WIDTH = width;
    HEIGHT = height;
    ctx.setUniform2f("screenSize", (float)WIDTH,   (float)HEIGHT);
}
