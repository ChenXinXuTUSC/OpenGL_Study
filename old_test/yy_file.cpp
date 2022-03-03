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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
void renderScene(Shader& shader);
void initTex();

const unsigned int SCR_WIDTH =  600;
const unsigned int SCR_HEIGHT = 600;


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;


float deltaTime = 0.0f;
float lastFrame = 0.0f;


glm::vec3 lightPos(1.25f, 4.0f, 1.25f);


glm::mat4 lightSpaceMatrix;

// yy's model
unsigned int planeVAO;
unsigned int planeVBO;

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;

unsigned int woodTexture;

void initYY();
void renderYY(Shader& shader);

// fgd's model
// no texture needed or randomly choose a texture to map
unsigned int fan_stubVAO = 0;
unsigned int fan_stubVBO = 0;
unsigned int fan_stubNUM = 0;
unsigned int fan_leafVAO = 0;
unsigned int fan_leafVBO = 0;
unsigned int fan_leafNUM = 0;
bool fan_on = false;
float fan_angle = 0.0f;
unsigned int whiteTexture = 0;
void initGD();
void renderGD(Shader& shader);

// other prototypes
void initScene();

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "YY 201936383308", NULL, NULL);
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

    // ����Ȳ���
    glEnable(GL_DEPTH_TEST);

    Shader shader(
        "../source/shader/YY_shader/shadow_mapping.vs", 
        "../source/shader/YY_shader/shadow_mapping.fs");
    Shader simpleDepthShader(
        "../source/shader/YY_shader/shadow_mapping_depth.vs",
        "../source/shader/YY_shader/shadow_mapping_depth.fs");

    // ��������
    initTex();
    initScene();
    // configure depth map FBO
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth texture
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // shader configuration
    shader.use();
    shader.setInt("diffuseTexture", 0);
    shader.setInt("shadowMap", 1);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ��Ⱦ�����ͼ
        glm::mat4 lightProjection, lightView;
        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 0.0, 1.0));
        lightSpaceMatrix = lightProjection * lightView;
        // render scene from light's point of view
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        renderScene(simpleDepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //���������ͼ��Ⱦ����
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //��Ⱦ����
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        //shader���������
        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("light.position", lightPos);
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        // ��Դ������
        shader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
        shader.setVec3("light.diffuse", 0.3f, 0.3f, 0.3f);
        shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        renderScene(shader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);

    glfwTerminate();
    return 0;
}

void initTex() {
    woodTexture = loadTexture("../images/wood.png");
    whiteTexture = loadTexture("../images/white.png");
}

void initScene()
{
    initYY();
    initGD();
}

void renderScene(Shader& shader)
{
    renderYY(shader);
    renderGD(shader);
}

void processInput(GLFWwindow* window)
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
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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

// =================================== our function start here ===================================
void initYY()
{
    // initialize (if necessary)
    if (planeVAO == 0)
    {
        //�ذ�
        float planeVertices[] = {
            // positions            // normals         // texcoords
            2.5f, -2.5f, 2.5f, 0.0f, 1.0f, 0.0f, 5.0f, 0.0f,
            -2.5f, -2.5f, 2.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -2.5f, -2.5f, -2.5f, 0.0f, 1.0f, 0.0f, 0.0f, 5.0f,

            2.5f, -2.5f, 2.5f, 0.0f, 1.0f, 0.0f, 5.0f, 0.0f,
            -2.5f, -2.5f, -2.5f, 0.0f, 1.0f, 0.0f, 0.0f, 5.0f,
            2.5f, -2.5f, -2.5f, 0.0f, 1.0f, 0.0f, 5.0f, 5.0f};

        // plane VAO
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        glBindVertexArray(0);
    }
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        Mesh tempCube("../model/cube/cube.obj");

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, tempCube.vertices.size() * sizeof(Vertex), &tempCube.vertices[0], GL_STATIC_DRAW);
        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
}

void renderYY(Shader &shader)
{
    //����ͼ
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    // �������
    shader.setVec3("material.ambient", 0.3f, 0.3f, 0.3f);
    shader.setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setVec3("material.specular", 0.3f, 0.3f, 0.3f); // specular lighting doesn't have full effect on this object's material
    shader.setFloat("material.shininess", 32.0f);
    //��Ⱦ
    // floor1
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //floor2
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //floor3
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    shader.setMat4("model", model);
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // cubes
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.5f));
    //����������ת
    model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
    shader.setMat4("model", model);
    //������
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void initGD()
{
    if (fan_stubVAO == 0)
    {
        Mesh fan_stub("../model/fan_stub/fan_stub.obj");
        fan_stubNUM = fan_stub.vertices.size();
        // init VAO
        glGenVertexArrays(1, &fan_stubVAO);
        glBindVertexArray(fan_stubVAO);
        // init VBO
        glGenBuffers(1, &fan_stubVBO);
        glBindBuffer(GL_ARRAY_BUFFER, fan_stubVBO);
        glBufferData(GL_ARRAY_BUFFER, fan_stub.vertices.size() * sizeof(Vertex), &fan_stub.vertices[0], GL_STATIC_DRAW);
        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
    if (fan_leafVAO == 0)
    {
        Mesh fan_leaf("../model/fan_leaf/fan_leaf.obj");
        fan_leafNUM = fan_leaf.vertices.size();
        // init VAO
        glGenVertexArrays(1, &fan_leafVAO);
        glBindVertexArray(fan_leafVAO);
        // init VBO
        glGenBuffers(1, &fan_leafVBO);
        glBindBuffer(GL_ARRAY_BUFFER, fan_leafVBO);
        glBufferData(GL_ARRAY_BUFFER, fan_leaf.vertices.size() * sizeof(Vertex), &fan_leaf.vertices[0], GL_STATIC_DRAW);
        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glBindVertexArray(0);
    }
}

void renderGD(Shader& shader)
{
    /**
     * @brief GD does not have a mapped texture
     * but setting a texture to GL_TEXTURE0 will
     * keep the compatibility
     */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, whiteTexture);
    shader.setVec3("material.ambient", 0.3f, 0.3f, 0.3f);
    shader.setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setVec3("material.specular", 0.3f, 0.3f, 0.3f);
    shader.setFloat("material.shininess", 32.0f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -2.5f, 1.25f));
    model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
    shader.setMat4("model", model);
    glBindVertexArray(fan_stubVAO);
    glDrawArrays(GL_TRIANGLES, 0, fan_stubNUM);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.7f, 1.25f));
    model = glm::rotate(model, glm::radians(float(glfwGetTime() * 60.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, -1.8f, 0.0f));
    model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
    shader.setMat4("model", model);
    glBindVertexArray(fan_leafVAO);
    glDrawArrays(GL_TRIANGLES, 0, fan_leafNUM);
}