#include <iostream>
#include <SDL2/SDL.h>
#include <glad/glad.h>

bool gQuit = false;
int gScreenHeight = 640;
int gScreenWidth = 480;
SDL_Window* gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;

void GetOpenGLVersionInfo() {
   std::cout << glGetString(GL_VENDOR) << std::endl;
   std::cout << glGetString(GL_RENDERER) << std::endl;
   std::cout << glGetString(GL_VERSION) << std::endl;
   std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void initialize() {
    // if(!SDL_Init(SDL_INIT_VIDEO) || gladLoadGLLoader(SDL_GL_GetProcAddress) == 0) {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL initializer failed." << std::endl;
        exit(1);
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4); 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1); 
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    gGraphicsApplicationWindow = SDL_CreateWindow("OpenGL Window",
                                                0, 0, gScreenWidth, gScreenHeight,
                                                SDL_WINDOW_OPENGL);

    if(gGraphicsApplicationWindow == nullptr) {
        std::cout << "SDL window failed to create." << std::endl;
        exit(1);
    }

    gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);
   if(gOpenGLContext == nullptr) {
        std::cout << "OpenGL context failed to create." << std::endl;
        exit(1);
    };

    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cout << "glad failed to initialize." << std::endl;
        exit(1);
    }

    GetOpenGLVersionInfo(); 
}

void process_inputs() {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT) {
            gQuit = true;
        } else if(e.type == SDL_KEYDOWN) {
            if(e.key.keysym.sym == SDLK_ESCAPE) {
                gQuit = true;
            }
        }
    }
}

void pre_draw() {}
void draw() {
    SDL_GL_SwapWindow(gGraphicsApplicationWindow);
}

void main_loop() {
    while(!gQuit) {
        process_inputs();
        pre_draw();
        draw(); 
    }
}

void cleanup() {
    std::cout << "cleanup process" << std::endl;
    SDL_DestroyWindow(gGraphicsApplicationWindow);
    SDL_Quit();
}

int main() {
  initialize();
  main_loop();
  cleanup();     
    
  return 0;
}
