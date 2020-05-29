#include "mainwindow.h"

#include <QApplication>
#include "glad/glad.h"
#include "GLFW/glfw3.h"  //最后包含
#include <iostream>
#include <math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//着色器
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
//const char *fragmentShaderSource = "#version 330 core\n"
//    "out vec4 FragColor;\n"
//    "void main()\n"
//    "{\n"
//    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//    "}\n\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = ourColor;\n"
    "}\n\0";

int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // glfw window creation
        // --------------------
        GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
        if (window == nullptr)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }


        //载入着色器
        //创建顶点着色器对象
        uint vertexShader = glad_glCreateShader(GL_VERTEX_SHADER);
        //将着色器源码附加到着色器对象
        glad_glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glad_glCompileShader(vertexShader);
        int sucess;
        char infoLog[512];
        glad_glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &sucess);
        if(!sucess)
        {
            glad_glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cout << "error::shader::vertex::compilation_failed\n" << infoLog << std::endl;

        }

        //片段着色器
        uint fragmentShader = glad_glCreateShader(GL_FRAGMENT_SHADER);
        glad_glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glad_glCompileShader(fragmentShader);
        glad_glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &sucess);
        if(!sucess)
        {
            glad_glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
            std::cout << "error::shader::vertex::compilation_failed\n" << infoLog << std::endl;
        }

        //uniform着色器

        //着色器程序
        /* 着色器程序对象(Shader Program Object)是多个着色器合并之后并最终链接完成的版本。
         * 如果要使用刚才编译的着色器我们必须把它们链接(Link)为一个着色器程序对象，然后在渲染
         * 对象的时候激活这个着色器程序。已激活着色器程序的着色器将在我们发送渲染调用的时候被使用。
         * 当链接着色器至一个程序的时候，它会把每个着色器的输出链接到下个着色器的输入。当输出和输入
         * 不匹配的时候，你会得到一个连接错误。
        */
        uint shaderProgram = glad_glCreateProgram();
        //把之前编译的说则其附加到程序对象
        glad_glAttachShader(shaderProgram, vertexShader);
        glad_glAttachShader(shaderProgram, fragmentShader);
        glad_glLinkProgram(shaderProgram);
        glad_glGetProgramiv(shaderProgram, GL_LINK_STATUS, &sucess);
        if(!sucess)
        {
            glad_glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        }
        glad_glUseProgram(shaderProgram);

        //已链接着色器到程序对象，现在可以删除
        glad_glDeleteShader(vertexShader);
        glad_glDeleteShader(fragmentShader);
        /*现在，我们已经把输入顶点数据发送给了GPU，并指示了GPU如何在顶点和片段着色器中处理它。
         * 就快要完成了，但还没结束，OpenGL还不知道它该如何解释内存中的顶点数据，以及它该如何
         * 将顶点数据链接到顶点着色器的属性上。我们需要告诉OpenGL怎么做。
         * */


        // set up vertex data (and buffer(s)) and configure vertex attributes
            // ------------------------------------------------------------------
            float vertices[] = {
                 0.5f,  0.5f, 0.0f,  // top right
                 0.5f, -0.5f, 0.0f,  // bottom right
                -0.5f, -0.5f, 0.0f,  // bottom left
                -0.5f,  0.5f, 0.0f   // top left
            };
            unsigned int indices[] = {  // note that we start from 0!
                0, 1, 3,  // first Triangle
                1, 2, 3   // second Triangle
            };
            unsigned int VBO, VAO, EBO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            //告诉OpenGL如何解析顶点数据
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
            // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
            glBindVertexArray(0);

            //设置线框模式画图
//            glad_glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //设置填充模式画图
            glad_glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            int nrAttributes;
            glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
            std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

        // render loop
        // -----------
        while (!glfwWindowShouldClose(window))
        {
            // input
            // -----
            processInput(window);

            // render
            // ------
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // draw our first triangle
            glUseProgram(shaderProgram);
            glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized


            //使用uniform改变颜色
            float timeValue = glfwGetTime();
            float greenValue = sin(timeValue) / 2.0f * 0.8f;
            int vertexColorLocation = glad_glGetUniformLocation(shaderProgram, "ourColor");
            glad_glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

            //glDrawArrays(GL_TRIANGLES, 0, 6);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        glad_glDeleteVertexArrays(1, &VAO);
        glad_glDeleteBuffers(1, &VBO);
        glad_glDeleteProgram(shaderProgram);

        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
        glfwTerminate();
        return 0;
    }

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void processInput(GLFWwindow *window)
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    // ---------------------------------------------------------------------------------------------
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
        std::cout << "current width" << width << ", current height " << height << std::endl;
    }
