#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <myMesh.h>

#include <iostream>
#include <string>

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

// lighting
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
    // ------------------------------------
    Shader blinnShader("../source/shader/vmodel.glsl", "../source/shader/fblinn.glsl");
    Shader textureShader("../source/shader/vtexture.glsl", "../source/shader/ftexture.glsl");

    Mesh ourCube("../model/cube/cube.obj");
    ourCube.addMapping("../images/container2.png", Texture::TEXType::DIFF);
    ourCube.addMapping("../images/container2_specular.png", Texture::TEXType::SPEC);

    Mesh ourRing("../model/cube/cube.obj");

    Mesh ourGround("../model/ground/ground.obj");
    ourGround.addMapping("../images/wood.png", Texture::TEXType::SPEC);

    // render loop
    // -----------
    // shader settings
    blinnShader.use();
    blinnShader.setFloat("material.shininess", 32.0f);
    blinnShader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
    blinnShader.setVec3("light.diffuse", 0.6f, 0.6f, 0.6f);
    blinnShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    blinnShader.setFloat("light.constant", 1.0f);
    blinnShader.setFloat("light.linear", 0.09f);
    blinnShader.setFloat("light.quadratic", 0.032f);

    textureShader.use();
    textureShader.setFloat("material.shininess", 32.0f);
    textureShader.setVec3("material.ambient", 0.5f, 0.475f, 0.0f);
    textureShader.setVec3("material.diffuse", 1.0f, 0.95f, 0.0f);
    textureShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    textureShader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
    textureShader.setVec3("light.diffuse", 0.6f, 0.6f, 0.6f);
    textureShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    textureShader.setFloat("light.constant", 1.0f);
    textureShader.setFloat("light.linear", 0.09f);
    textureShader.setFloat("light.quadratic", 0.032f);

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        // ----------blinnShader----------
        blinnShader.use();
        blinnShader.setVec3("light.position", lightPos);
        blinnShader.setVec3("viewPos", camera.Position);
        blinnShader.setInt("blinn", blinn);

        // ----------textureShader----------
        textureShader.use();
        textureShader.setVec3("light.position", lightPos);
        textureShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        blinnShader.use();
        blinnShader.setMat4("projection", projection);
        blinnShader.setMat4("view", view);
        textureShader.use();
        textureShader.setMat4("projection", projection);
        textureShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        blinnShader.use();
        blinnShader.setMat4("model", model);
        textureShader.use();
        textureShader.setMat4("model", model);

        // draw all the models
        glEnable(GL_CULL_FACE);
        // blinnShader.use();
        // ourCube.Draw(blinnShader);

        textureShader.use();
        ourRing.Draw(textureShader);

        glDisable(GL_CULL_FACE);
        blinnShader.use();
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(2.5f, 1.0f, 2.5f));
        blinnShader.setMat4("model", model);
        ourGround.Draw(blinnShader);

        // draw a second one
        // model = glm::translate(model, glm::vec3(3.0f, 0.0f, 3.0f));
        // blinnShader.setMat4("model", model);

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
