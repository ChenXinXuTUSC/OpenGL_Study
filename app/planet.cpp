#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <myMesh.h>
#include <myModel.h>

#include <iostream>
#include <string>
#include <random>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting related variables
glm::vec3 lightPos(1.8f, 1.0f, 3.0f);
bool blinn = false;
bool blinnKeyPressed = false;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // build and compile our shader zprogram
    /**----------------------------------------------------------------
     * @brief 
     * 4 point-lights is set in the shader, please do the corresponding
     * changes in the render loop.
     * ----------------------------------------------------------------
     */

    // shader settings
    Shader multiShader("../shader/shader_vertex/vubo.glsl", "../shader/shader_fragment/fubo.glsl");
    Shader lampShader("../shader/shader_vertex/vlamp2.glsl", "../shader/shader_fragment/flamp2.glsl");

    unsigned int uniformBlockIndex_multiShader = glGetUniformBlockIndex(multiShader.ID, "Matrices");
    unsigned int uniformBlockIndex_lampShader = glGetUniformBlockIndex(lampShader.ID, "Matrices");

    glUniformBlockBinding(multiShader.ID, uniformBlockIndex_multiShader, 0);
    glUniformBlockBinding(lampShader.ID, uniformBlockIndex_lampShader, 0);

    unsigned int UBO;
    glGenBuffers(1, &UBO);

    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, 2 * sizeof(glm::mat4));

    // model import
    Model testDoll("../model/nanosuit", "nanosuit");
    Mesh ourCube("../model/cube/cube.obj");
    ourCube.addMapping("../images/container2.png", Texture::TEXType::DIFF);
    ourCube.addMapping("../images/container2_specular.png", Texture::TEXType::SPEC);

    Mesh planet("../model/planet/planet.obj");
    planet.addMapping("../model/planet/planet_Quom1200.png", Texture::TEXType::DIFF);
    Mesh rock("../model/rock/rock.obj");
    rock.addMapping("../model/rock/Rock-Texture-Surface.jpg", Texture::TEXType::DIFF);
    // rock positions
    std::srand(glfwGetTime());

    unsigned int amount = 1000;
    std::vector<glm::mat4> modelMatrices(amount);
    float radius = 50.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; ++i)
    {
        /**
         * @brief if you are using a relatively new version
         * of GLM, remeber to initialize matrix with 1.0f!
         * or every element of the matrix will be uncertain
         */
        glm::mat4 model(1.0f);
        // translationg to a circle
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // 让行星带的高度比x和z的宽度要小
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. rotate: rotate with a random axis
        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 3. scale: scale between 0.01 and 0.25
        float scale = (rand() % 25) / 100.0f + 0.1;
        model = glm::scale(model, glm::vec3(scale));

        // 4. add transformation to matrices
        modelMatrices[i] = model;
    }


    // shader settings
    // positions of the point lights
    std::vector<glm::vec3> pointLightPositions =
    {
        glm::vec3(10.0f, 1.0f, 10.0f),
        glm::vec3(10.0f, 1.0f, -10.0f),
        glm::vec3(-10.0f, 1.0f, 10.0f),
        glm::vec3(-10.0f, 1.0f, -10.0f)
    };

    unsigned int pointLightNums = pointLightPositions.size();
    multiShader.use();
    multiShader.setFloat("material.shininess", 64.0f);
    multiShader.setVec3("dirLight.direction", 0.0f, -1.0f, 0.0f);
    multiShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
    multiShader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
    multiShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
    for (unsigned int i = 0; i < pointLightNums; ++i)
    {
        multiShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.05f, 0.05f, 0.05f);
        multiShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.4f, 0.4f, 0.4f);
        multiShader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
        multiShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
        multiShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
        multiShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
    }

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render clear
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        // shader settings
        // model/view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        glBindBuffer(GL_UNIFORM_BUFFER, UBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        multiShader.use();
        // point lights' position settings
        for (unsigned int i = 0; i < pointLightNums; ++i)
        {
            multiShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
        }
        multiShader.setVec3("viewPos", camera.Position);
        multiShader.setInt("blinn", blinn);

        // draw all the models
        glEnable(GL_CULL_FACE);

        // draw the doll
        multiShader.use();
        multiShader.setMat4("model", model);
        testDoll.Draw(multiShader);

        // draw the planet
        // model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));
        // multiShader.setMat4("model", model);
        // planet.Draw(multiShader);

        // draw all rocks that surround with planet
        for (unsigned int i = 0; i < amount; ++i)
        {
            multiShader.setMat4("model", modelMatrices[i]);
            rock.Draw(multiShader);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn = !blinn;
        blinnKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
        blinnKeyPressed = false;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
