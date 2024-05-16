#include <iostream>



#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <string>
#include <sstream>


struct ShaderProgramSources
{
    std::string VertexSource;
    std::string FragmentSource;
};


static ShaderProgramSources ParseShader(const std::string& filePath)
{
    std::ifstream stream(filePath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0,FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompiledShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    std::cout << "id = " << id << std::endl;
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compiled " <<
            (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " Shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompiledShader(GL_VERTEX_SHADER,vertexShader);
    unsigned int fs = CompiledShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);


    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;
    //First you need to create a valid OpenGL rendering context and call glewInit() to initialize the extension entry points. 

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[] = {
            -0.5f, -0.5f, //0
             0.0f, 0.5f, //1
             0.5f, -0.5f, //2
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions,GL_STATIC_DRAW);

    //p99 start
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    //p99end

    ShaderProgramSources source = ParseShader("res/shaders/Basic.shader");
    std::cout << "Vertex********" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "Fragment*******" << std::endl;
    std::cout << source.FragmentSource << std::endl;

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //(const void*)8;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //glBegin(GL_TRIANGLES);
        //glVertex2d(-0.5f, -0.5f);

        glDrawArrays(GL_TRIANGLES,0,3);
        


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}