#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES

#define ASSERT(x) if (!(x)) __debugbreak();
#define glCall(x) glClearError();\
    x;\
    ASSERT(glLogCall(#x, __FILE__, __LINE__));

static void glClearError() 
{
    while (glGetError());
}

static bool glLogCall(const char* function, const char* file, int line)
{
    if (GLenum err = glGetError()) 
    {
        std::cout << "[OpenGL Error] (0x" << std::hex << err << "):" << function << " " << file << ":" << std::dec << line << std::endl;
        return false;
    }
    return true;
}

enum class ShaderType
{
    NONE = -1, VERTEX = 0, FRAGMENT = 1
};

struct ShaderProgramSources
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSources ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                //set mode to vertex
                type = ShaderType::VERTEX;

            }
            else if (line.find("fragment") != std::string::npos)
            {
                //set mode to fragment
                type = ShaderType::FRAGMENT;

            }
        }
        else if (type != ShaderType::NONE) 
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}

static void LogShaderCompilationError(unsigned int type, unsigned int shader_id)
{
    int length;
    std::string type_str = (type == GL_FRAGMENT_SHADER ? "fragment" : (type == GL_VERTEX_SHADER ? "vertex" : "unrecognised"));
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
    char* log = (char*)alloca(length * sizeof(char));
    glGetShaderInfoLog(shader_id, length, &length, log);
    std::cout << "Compilation of " << type_str << " shader failed." << std::endl;
    std::cout << log << std::endl;
    glDeleteShader(shader_id);
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);

    glCompileShader(id);

    int compilation_result;

    glGetShaderiv(id, GL_COMPILE_STATUS, &compilation_result);

    if (compilation_result != GL_TRUE)
    {
        LogShaderCompilationError(type, id);
        return 0;
    }

    return id;

}

static void LogProgramError(const char *message, unsigned int program_id)
{
    std::cout << message << std::endl;
    int length;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);
    char* log = (char*)alloca(length * sizeof(char));
    glGetProgramInfoLog(program_id, length, &length, log);
    std::cout << log << std::endl;
    glDeleteProgram(program_id);
}

static int CreateShaders(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    int validation_result, link_result;

    glGetProgramiv(program, GL_VALIDATE_STATUS, &validation_result);
    glGetProgramiv(program, GL_LINK_STATUS, &link_result);

    if (link_result != GL_TRUE)
    {
        std::string message = "Program link failed.";
        LogProgramError(message.c_str(), program);
        return 0;
    }

    if (validation_result != GL_TRUE)
    {
        std::string message = "Program validation failed.";
        LogProgramError(message.c_str(), program);
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 640, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        printf("ERROR");
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

   /* float positions[] = {
        -0.5f,  0.5f,
        -0.5f, -0.5f,
         0.5f,  0.5f,
         0.5f, -0.5f
    };
    unsigned int indices[]{
        0, 1, 2,
        1, 2, 3
    };*/

    float angle_inc = 10.0f;
    float angle = 0.0f;
    const float PI = 2 * acos(0.0f);

    int pos_count = (int)(360.0f / angle_inc);
    int index_count = (pos_count) * 3;

    float positions[1000];
    
    unsigned int indices[1000];

    std::cout << "pos: " << pos_count << std::endl << "index: " << index_count << std::endl;

    //pos[0,1] reserved for center
    positions[0] = 0.0f;
    positions[1] = 0.0f;
    pos_count += 1;

    for (int i = 0; i < pos_count * 2 - 2; i += 2)
    {
        positions[i + 2] = cos(angle / 360 * 2 * PI) / 2;
        positions[i + 3] = sin(angle / 360 * 2 * PI) / 2;
        angle += angle_inc;
        //std::cout << "x: " << positions[i + 2] << "\ty: " << positions[i + 3] << " : angle: " << angle << std::endl;
    }

    for (int i = 0; i < pos_count; i += 1)
    {
        indices[3 * i] = 0;
        indices[3 * i + 1] = i + 1;
        indices[3 * i + 2] = i + 2;
        if (i == pos_count - 2) indices[3 * i + 2] = 1;
    }

    unsigned int vao;

    glCall(glGenVertexArrays(1, &vao));
    glCall(glBindVertexArray(vao));

    unsigned int buffer;

    glCall(glGenBuffers(1, &buffer));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    glCall(glBufferData(GL_ARRAY_BUFFER, pos_count * 2 * sizeof(float), positions, GL_STATIC_DRAW));

    glCall(glEnableVertexAttribArray(0));
    glCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));

    unsigned int ibo;

    glCall(glGenBuffers(1, &ibo));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    ShaderProgramSources sps = ParseShader("res/shaders/Basic.shader");

    unsigned int shaders = CreateShaders(sps.VertexSource, sps.FragmentSource);
    glCall(glUseProgram(shaders));

    glCall(GLint u_timeLoc = glGetUniformLocation(shaders, "u_time"));
    ASSERT(u_timeLoc != -1);


    //unbind everything
    glCall(glBindVertexArray(0));
    glCall(glUseProgram(0));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


    float r = 0.0f, i = 0.05f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        glViewport(0, 0, width, height);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glCall(glUseProgram(shaders));
        glCall(glBindVertexArray(vao));
        glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

        float time = glfwGetTime();
        
        glCall(glUniform1f(u_timeLoc, time));


        glCall(glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr));


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();


        
    }

    glDeleteProgram(shaders);

    glfwTerminate();
    return 0;
}