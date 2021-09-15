#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <math.h>
#include "shader.h"
#include "camera.h"

// call back functions
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void keyboardProcess(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double currX, double currY);
void scrol_callback(GLFWwindow *window, double currX, double currY);

// global settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGH = 600;
// delta time
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currFrame = 0.0f;
// camera
Camera ourCamera(glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 1.0f, 0.0f),
                 0.0f, 0.0f);
float lastX = float(SCR_WIDTH / 2);
float lastY = float(SCR_HEIGH / 2);
bool firstMove = true;
// shader

// draw functions
const int MAX_LEVEL = 3;
void divideTriangle(Shader* shader, const glm::vec3& p1, const float len, int level);

int main(int argc, char **argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT);
#endif

    // window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGH, "LearOpenGL", NULL, NULL);
    if (window == nullptr)
    {
        std::cout << "Failed to create window...\n"
                  << std::endl;
        glfwTerminate();
        return -1;
    }
    // register window and callback functions
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scrol_callback);

    // load all function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Faield to initialize GLAD...\n";
        return -1;
    }

    // global OpenGL configure
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // compute sphere vertices
    float vertices[] =
    {
        // coordinate                                           // color
        0.000000f, std::sqrt(2.0f / 3.0f), 0.000000f,           1.0f, 0.0f, 0.0f,
        0.000000f, 0.000000f, -1.0f / std::sqrt(3.0f),          0.0f, 1.0f, 0.0f,
        -0.500000f, 0.000000f, 1.0f / (2.0f * std::sqrt(3.0f)), 0.0f, 0.0f, 1.0f,
        0.500000f, 0.000000f,  1.0f / (2.0f * std::sqrt(3.0f)), 1.0f, 1.0f, 0.0f
    };
    // compute sphere indices
    unsigned int indices[] = 
    {
        0, 1, 2,
        0, 1, 3,
        0, 2, 3,
        1, 2, 3
    };
    // glm::vec3 tpye vertices
    glm::vec3 trianglePoses[] =
    {
        glm::vec3(0.000000f, std::sqrt(2.0f / 3.0f), 0.000000f),
        glm::vec3(0.000000f, 0.000000f, -1.0f / std::sqrt(3.0f)),
        glm::vec3(-0.500000f, 0.000000f, 1.0f / (2.0f * std::sqrt(3.0f))),
        glm::vec3(0.500000f, 0.000000f, 1.0f / (2.0f * std::sqrt(3.0f))),
    };

    // 开始分配缓存对象
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    // 绑定顶点数组对象，启用当前VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertices),
                 vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(indices),
                 indices, GL_STATIC_DRAW);

    // set vertex pointer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // build and compile shader program
    // remember to replace with your own shader!
    Shader* ourShader = new Shader("../source/shader/vsierpin.glsl", "../source/shader/fsierpin.glsl");
    ourShader->use();

    // setting initial transformation
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(ourCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGH), 0.1f, 100.0f);
    glm::mat4 viewing = ourCamera.GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    ourShader->setMat4("model", model);
    ourShader->setMat4("view", viewing);
    ourShader->setMat4("projection", projection);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        currFrame = glfwGetTime();
        deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;

        // process input
        keyboardProcess(window);

        ourShader->use();
        // setting transformation
        // ourShader.setMat4("model", model);
        ourShader->setMat4("view", ourCamera.GetViewMatrix());
        projection = glm::perspective(glm::radians(ourCamera.Zoom), float(SCR_WIDTH) / float(SCR_HEIGH), 0.1f, 100.0f);
        ourShader->setMat4("projection", projection);

        // rendering
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        divideTriangle(ourShader, trianglePoses[0], 1.0f, 1);
        // glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void keyboardProcess(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ourCamera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ourCamera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        ourCamera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        ourCamera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}
void mouse_callback(GLFWwindow *window, double currX, double currY)
{
    if (firstMove)
    {
        lastX = currX;
        lastY = currY;
        firstMove = false;
    }
    ourCamera.ProcessMouseMovement(currX - lastX, lastY - currY, true);
    lastX = currX;
    lastY = currY;
}
void scrol_callback(GLFWwindow *window, double currX, double currY)
{
    ourCamera.ProcessMouseScroll(currY);
}

void divideTriangle(Shader* shader, const glm::vec3& p1, const float len, int level)
{
    if (level == MAX_LEVEL)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, p1 - glm::vec3(0.0f, std::sqrt(2.0f / 3.0f), 0.0f));
        model = glm::scale(model,
                           glm::vec3(1.0f / float(1 << (MAX_LEVEL - 1)),
                                     1.0f / float(1 << (MAX_LEVEL - 1)),
                                     1.0f / float(1 << (MAX_LEVEL - 1))));
        shader->setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
        return;
    }
    divideTriangle(
        shader,
        p1,
        len / 2.0f,
        level + 1);
    divideTriangle(
        shader,
        p1 + glm::vec3(0.0f, -len * std::sqrt(2.0f / 3.0f) / 2.0f, -len / (2.0f * std::sqrt(3.0f))),
        len / 2.0f,
        level + 1);
    divideTriangle(
        shader,
        p1 + glm::vec3(-len / 4.0f, -len * std::sqrt(2.0f / 3.0f) / 2.0f, len / (4.0f * std::sqrt(3.0f))),
        len / 2.0f,
        level + 1);
    divideTriangle(
        shader,
        p1 + glm::vec3(len / 4.0f, -len * std::sqrt(2.0f / 3.0f) / 2.0f, len / (4.0f * std::sqrt(3.0f))),
        len / 2.0f,
        level + 1);
    // divideTriangle(
    //     shader,
    //     p1 + glm::vec3(-len / 4.0f, -len / 2.0f, -len / 4.0f),
    //     len / 2.0f,
    //     level + 1);
}