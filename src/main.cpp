#include <iostream>
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <vector>
#include <fstream>

// SDL2 window parameters
bool gQuit = false;
int gScreenHeight = 480;
int gScreenWidth = 640;
SDL_Window* gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;

// VAO/VBO handles
GLuint g_VBO = 0;
GLuint g_VAO = 0;

// program object for shaders
GLuint g_shader_program = 0;
GLuint g_graphics_shader_pipeline = 0;

std::string load_shader_src(const std::string& filename) {
    std::string ret = "";
    std::string line = "";
    std::ifstream raw_file(filename.c_str());
    if(raw_file.is_open()) {
        while(std::getline(raw_file, line)) {
            ret += line + "\n";
        }
    raw_file.close();
    } else {
        std::cout << "Failed to open file: " << filename << std::endl;
    }
    return ret;
}

void GetOpenGLVersionInfo() {
   std::cout << glGetString(GL_VENDOR) << std::endl;
   std::cout << glGetString(GL_RENDERER) << std::endl;
   std::cout << glGetString(GL_VERSION) << std::endl;
   std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void vertex_specification() {
    // vertex data starts on the CPU
    const std::vector<GLfloat> vertex_positions {
        -0.8f, -0.8f, -0.0f,
        0.8f,  -0.8f,  0.0f,
        0.0f,   0.8f,  0.0f
    };

    // vertex data is sent to the GPU
    // step 1: create VAO
    glGenVertexArrays(1, &g_VAO);
    glBindVertexArray(g_VAO);

    // step 2: create VBO and copy over data
    glGenBuffers(1, &g_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertex_positions.size() * sizeof(GLfloat),
        vertex_positions.data(),
        GL_STATIC_DRAW
    );

    // step 3: enable the desired attribute (for position) in the VAO
    glEnableVertexAttribArray(0);
    
    // step 4: specify vertex data format in VAO 
    glVertexAttribPointer(
        0,
        3, 
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (GLvoid*)0
    );

    // the data should be fully copied over to GPU memory
    //step 6: unbind the VBO/VAO for the next draw call
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
}

GLuint compile_shader(GLuint type, const std::string& raw_src) {
    GLuint shader_object; 
    if(type == GL_VERTEX_SHADER) {
        shader_object = glCreateShader(GL_VERTEX_SHADER);
    } else if(type == GL_FRAGMENT_SHADER) {
        shader_object = glCreateShader(GL_FRAGMENT_SHADER);
    } 
    // compile the shader
   
    const char* src = raw_src.c_str();
    glShaderSource(shader_object, 1, &src, nullptr);
    glCompileShader(shader_object);
    return shader_object; 
}


GLuint create_shader_program(const std::string& vertex_shader_src,
                             const std::string& fragment_shader_src) {

    // glCreateProgram creates empty program object (which is our pipeline)
    // we need to populate it with our shaders

    GLuint program_object = glCreateProgram();
    
    // create shader of type VERTEX_SHADER using the provided source code
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_src); 
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

    // attach the shaders to the program object
    glAttachShader(program_object, vertex_shader);
    glAttachShader(program_object, fragment_shader);

    // link the program object
    glLinkProgram(program_object);

    // TODO: still need to delete and detach the shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program_object;
}

void create_graphics_pipeline() {
    std::string vertex_shader_src = load_shader_src("./shaders/vert.glsl");
    std::string fragment_shader_src = load_shader_src("./shaders/frag.glsl");
    g_graphics_shader_pipeline = create_shader_program(vertex_shader_src, fragment_shader_src);
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

// pre_draw is responsible for setting oGL state
void pre_draw() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glViewport(0, 0, gScreenWidth, gScreenHeight);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glClearColor(0.f, 0.2f, 0.2f, 1.f); 

    glUseProgram(g_graphics_shader_pipeline);
}

void draw() {
    glBindVertexArray(g_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_VBO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void main_loop() {
    while(!gQuit) {
        process_inputs();
        pre_draw();
        draw(); 
        SDL_GL_SwapWindow(gGraphicsApplicationWindow); // update the screen
    }
}

void cleanup() {
    std::cout << "cleanup process" << std::endl;
    SDL_DestroyWindow(gGraphicsApplicationWindow);
    SDL_Quit();
}

int main() {
  initialize();
  vertex_specification();
  create_graphics_pipeline();
  main_loop();
  cleanup();     
    
  return 0;
}
