#version 330 core

uniform int itr;
uniform float zoom;
uniform vec2 screenSize;
uniform vec2 offset;

int n = 0;
float threshold = 100.0;
float flmz(float a){
        if(a<0.0){
        return 0.0;
    }else{
        return a;
    }
}
//colors
float red(int j){
    return flmz(sin(float(j)/100.0));
};
float gre(int j){
    return flmz(sin(float(j)/100.0+1.0));
};
float blu(int j){
    return flmz(sin(float(j)/100.0+2.0));
};

int mandelbrot(vec2 c) {
    vec2 z = vec2(0.0,0.0);
    for(int i = 0; i < itr; i++){
        vec2 znew;
        znew.x = (z.x * z.x) - (z.y * z.y) + c.x;
        znew.y = (2.0 * z.x * z.y) +c.y;
        z = znew;
        if((z.x * z.x) + (z.y * z.y) > threshold)break;
        n++;
    }
    return n;
}

vec4 map_to_color(int t) {
    //float r = 9.0 * (1.0 - t) * t * t * t;
    //float g = 15.0 * (1.0 - t) * (1.0 - t) * t * t;
    //float b = 8.5 * (1.0 - t) * (1.0 - t) * (1.0 - t) * t;
    if(t == itr){
        return vec4(0.0,0.0,0.0,1.0);
    }
    return vec4(red(t),gre(t),blu(t),1.0);
    //return vec4(r, g, b, 1.0);
}

void main() {
    vec2 coord = vec2(gl_FragCoord.xy);
    int t = mandelbrot(((coord - screenSize/2)/zoom) - offset);
    if(gl_FragCoord.x < 40){
        gl_FragColor = vec4(1.0);
    }
    gl_FragColor = map_to_color(t);
}