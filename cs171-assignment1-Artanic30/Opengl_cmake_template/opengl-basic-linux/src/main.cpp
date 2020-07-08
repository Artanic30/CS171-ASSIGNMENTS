#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utils/shader.h>
#include <utils/camera.h>

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
std::string GetCurrentWorkingDir();
void drawTri(unsigned int VAO, Shader shader);
void drawTET(unsigned int VAO, Shader shader);
void drawCube(unsigned int VAO, Shader shader);
void drawSphere(unsigned int VAO, Shader shader, int accuracy);
void initPMV(Shader shader);
void changePMV(Shader shader);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void genSphere(float* vertices, int accuracy, float radius);
void attachNormal(float *vertices, float *ori_vertices, int ori_length);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
Camera *camera;
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;


int main()
{
    std::string root_dir = GetCurrentWorkingDir();
    int len = root_dir.length();
    // cmakeLists.txt所在文件目录绝对位置
    root_dir = root_dir.substr(0, len - 18);
    std::string root_location = root_dir +  "/src/";
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 设置每个像素采样点
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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
    glfwSetCursorPosCallback(window, mouse_callback);
    // 隐藏光标并且捕获
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // build and compile our shader program
    // ------------------------------------
    Shader ourShader((root_location + "shader.vs").c_str(), (root_location + "shader.fs").c_str());

    // 设置三角形顶点坐标
    // set up vertex data (and buffer(s)) and configure vertex attributes
    float raw_verticesTri[] = {
            1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f
    };
    float verticesTri[9 * 2];
    attachNormal(verticesTri, raw_verticesTri, 9);
    unsigned int VBO, VAO_TRI;
    glGenVertexArrays(1, &VAO_TRI);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO_TRI);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTri), verticesTri, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float raw_verticesTET[] = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, -1.0f, 0.0f,

            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,

            1.0f, 0.0f, 0.0f,
            0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,

            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, -1.0f, 0.0f
    };
    float verticesTET[12 * 3 * 2];
    attachNormal(verticesTET, raw_verticesTET, 12 * 3);
    unsigned int VAO_TET;
    glGenVertexArrays(1, &VAO_TET);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO_TET);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTET), verticesTET, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    float raw_verticesCube[] = {
            // 上表面
            -0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f,
            0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f,
            0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
    };
    float verticesCube[36 * 3 * 2];
    attachNormal(verticesCube, raw_verticesCube, 36 * 3);
    /*
    for (int i = 0; i < 36 * 3 * 2; i += 3) {
        std::cout << "point" << i / 3 << ": " <<verticesCube[i] << ", " << verticesCube[i + 1] << ", " << verticesCube[i + 2]<< ","<<std::endl;
    }
     */
    unsigned int VAO_CUBE;
    glGenVertexArrays(1, &VAO_CUBE);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO_CUBE);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);
    // 设置输入点坐标与shader内接收关系
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    int accuracy = 80;
    int sphere_points_number = accuracy * 72 * accuracy;
    float raw_verticesSp[sphere_points_number];
    genSphere(raw_verticesSp, accuracy, 1);
    float verticesSp[sphere_points_number * 2];
    attachNormal(verticesSp, raw_verticesSp, sphere_points_number);
    /*
    for (int i = 0; i < accuracy * 144; i += 3) {
        std::cout << "point" << i / 3 << ": " <<verticesSp[i] << ", " << verticesSp[i + 1] << ", " << verticesSp[i + 2]<< ","<<std::endl;
    }
    */
    unsigned int VAO_Sphere;
    glGenVertexArrays(1, &VAO_Sphere);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO_Sphere);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSp), verticesSp, GL_STATIC_DRAW);
    // 设置输入点坐标与shader内接收关系
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    initPMV(ourShader);

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    // 开启多采样
    glEnable(GL_MULTISAMPLE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        changePMV(ourShader);

        drawTET(VAO_TET, ourShader);

        drawSphere(VAO_Sphere, ourShader, accuracy);

        drawCube(VAO_CUBE, ourShader);
        drawTri(VAO_TRI, ourShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO_CUBE);
    glDeleteVertexArrays(1, &VAO_TRI);
    glDeleteVertexArrays(1, &VAO_Sphere);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    delete camera;
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, deltaTime);
}
// 处理鼠标移动
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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

    camera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// 返回当前文件夹位置绝对值
std::string GetCurrentWorkingDir() {
    char buff[FILENAME_MAX];
    GetCurrentDir( buff, FILENAME_MAX );
    std::string current_working_dir(buff);
    return current_working_dir;
}
// 画三角形
void drawTri(unsigned int VAO, Shader shader) {
    // model projection view 初始化值
    glm::mat4 model = glm::mat4(1.0f);

    shader.use();
    model = glm::translate(model, glm::vec3(1.5f, -1.5f, 1.5f));
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void drawTET(unsigned int VAO, Shader shader) {
    // model projection view 初始化值
    glm::mat4 model = glm::mat4(1.0f);

    shader.use();
    model = glm::translate(model, glm::vec3(-1.5f, -1.5f, -1.5f));
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 21);
};

// 画立方体
void drawCube(unsigned int VAO, Shader shader) {
    // model 初始化值
    glm::mat4 model = glm::mat4(1.0f);

    shader.use();
    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(2.0f, 2.0f, 2.0f));
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawSphere(unsigned int VAO, Shader shader, int accuracy) {
    // model 初始化值
    glm::mat4 model = glm::mat4(1.0f);

    shader.use();
    model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, accuracy * 24 * accuracy);
}
// 初始化视角和光照
void initPMV(Shader shader) {
    camera = new Camera (glm::vec3(0.0f, 0.0f, 3.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    shader.use();
    shader.setMat4("projection", projection);
    shader.setVec3("objectColor", 0.0f, 0.0f, 1.0f);
    shader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
    shader.setVec3("lightPos", 0.0f, 6.0f, 0.0f);
    shader.setVec3("viewPos", camera->Position);
}
// 根据摄像机视角变换
void changePMV(Shader shader) {
    glm::mat4 view;
    view = camera->GetViewMatrix();
    shader.use();
    // 更新shader中观察坐标系
    shader.setMat4("view", view);
    // shader更新视角位置
    shader.setVec3("viewPos", camera->Position);
}
// 生成圆坐标
void genSphere(float* vertices, int accuracy, float radius) {
    auto points = new glm::vec3*[accuracy + 1];
    double pi_step1 = 3.1415926 / accuracy;
    double pi_step2 = 3.1415926 / accuracy / 2;
    for (int i = 0;i <= accuracy; i++) {
        points[i] = new glm::vec3[accuracy * 2];
        // std::cout << "i: " << i << std::endl;
        for (int j = 0; j < accuracy * 2; ++j) {
            points[i][j] = glm::vec3(cos(pi_step1 * j)*cos(pi_step2 * i)*radius, sin(j * pi_step1) * cos(pi_step2 * i)*radius, sin(pi_step2 * i)*radius);
            // std::cout << points[i][j].x << " "<< points[i][j].y << " "<<points[i][j].z<< std::endl;
            // std::cout <<"parameters: "<< cos(pi_step1 * j) << " "<< sin(pi_step1 * j) << " "<<cos(pi_step2 * i)<< " "<<sin(pi_step2 * i) << std::endl;
        }
        // std::cout << "endline." << std::endl;
    }
    std::vector<float> faces;
    faces.reserve(accuracy);
    // dealing with the middle circle
    for (int k = 0; k < accuracy; ++k) {
        // std::cout << "level" << k << ": " << std::endl;
        for (int i = 0;i < accuracy * 2;i++) {
            // lower based faces
            // lower left
            faces.push_back(points[k][i].x);
            faces.push_back(points[k][i].y);
            faces.push_back(points[k][i].z);
            // std::cout << "face up" << k * accuracy * 2 + i << ": " << std::endl;
            // std::cout << points[k][i].x << " "<< points[k][i].y << " "<<points[k][i].z<< std::endl;

            // std::cout << points[k][i + 1].x << " "<< points[k][i + 1].y << " "<<points[k][i + 1].z<< std::endl;

            // lower right
            if (i == accuracy * 2 - 1) {
                faces.push_back(points[k][0].x);
                faces.push_back(points[k][0].y);
                faces.push_back(points[k][0].z);
            } else {
                faces.push_back(points[k][i + 1].x);
                faces.push_back(points[k][i + 1].y);
                faces.push_back(points[k][i + 1].z);
            };

            // higher left
            faces.push_back(points[k + 1][i].x);
            faces.push_back(points[k + 1][i].y);
            faces.push_back(points[k + 1][i].z);
            // std::cout << points[k + 1][i].x << " "<< points[k + 1][i].y << " "<<points[k + 1][i].z<< std::endl;


            // higher based faces
            // lower left
            faces.push_back(points[k + 1][i].x);
            faces.push_back(points[k + 1][i].y);
            faces.push_back(points[k + 1][i].z);
            // std::cout << "face down" << k * accuracy * 2 + i << ": " << std::endl;
            // std::cout << points[k + 1][i].x << " "<< points[k + 1][i].y << " "<<points[k + 1][i].z<< std::endl;

            // higher left
            if (i == accuracy * 2 - 1) {
                faces.push_back(points[k][0].x);
                faces.push_back(points[k][0].y);
                faces.push_back(points[k][0].z);
            } else {
                faces.push_back(points[k][i + 1].x);
                faces.push_back(points[k][i + 1].y);
                faces.push_back(points[k][i + 1].z);
            }

            // lower right
            if (i == accuracy * 2 - 1) {
                faces.push_back(points[k + 1][0].x);
                faces.push_back(points[k + 1][0].y);
                faces.push_back(points[k + 1][0].z);
            } else {
                faces.push_back(points[k + 1][i + 1].x);
                faces.push_back(points[k + 1][i + 1].y);
                faces.push_back(points[k + 1][i + 1].z);
            }
            // std::cout << points[k + 1][i + 1].x << " "<< points[k + 1][i + 1].y << " "<<points[k + 1][i + 1].z<< std::endl;

            // std::cout << points[k][i + 1].x << " "<< points[k][i + 1].y << " "<<points[k][i + 1].z<< std::endl;
        }
    }
    int size = faces.size();
    for (int k = 0; k < size; k += 3) {
        faces.push_back(-faces[k]);
        faces.push_back(faces[k + 1]);
        faces.push_back(-faces[k + 2]);
    }
    std::copy(faces.begin(), faces.end(), vertices);
    /*
    for (int l = 0; l < faces.size(); ++l) {
        vertices[l] = faces[l];
    }
     */
    for (int i = 0;i <= accuracy; i++) {
        delete [] points[i];
    }
    delete [] points;
}

void attachNormal(float *vertices, float *ori_vertices, int ori_length) {
    std::vector<float> tem_vertices;
    tem_vertices.reserve(3);
    for (int i = 0; i < ori_length; i+=9) {
        // std::cout << "i value: " << i << std::endl;
        glm::vec3 first(ori_vertices[i], ori_vertices[i + 1], ori_vertices[i + 2]);
        glm::vec3 second(ori_vertices[i + 3], ori_vertices[i + 4], ori_vertices[i + 5]);
        glm::vec3 third(ori_vertices[i + 6], ori_vertices[i + 7], ori_vertices[i + 8]);
        glm::vec3 first_vector = first - second, second_vector = first - third;
        glm::vec3 cross_result = glm::cross(first_vector, second_vector);
        cross_result = glm::normalize(cross_result);
        // 确保法向量在物体本地坐标系中由原点指向外侧
        if (glm::dot(first, cross_result) < 0) {
            cross_result = -cross_result;
        }
        // std::cout << "first vector: " << first.x << " " << first.y << " " << first.z << std::endl;
        // std::cout << "second vector: " << second.x << " " << second.y << " " << second.z << std::endl;
        // std::cout << "normal vector: " << cross_result.x << " " << cross_result.y << " " << cross_result.z << std::endl;
        for (int j = 0; j < 3; ++j) {
            // std::cout << "j value: " << j << std::endl;
            tem_vertices.push_back(ori_vertices[i + j * 3]);
            // std::cout << "checkpoint2" <<std::endl;
            tem_vertices.push_back(ori_vertices[i + j * 3 + 1]);
            // std::cout << "checkpoint3" <<std::endl;
            tem_vertices.push_back(ori_vertices[i + j * 3 + 2]);
            // std::cout << "checkpoint1" <<std::endl;
            tem_vertices.push_back(cross_result.x);
            // std::cout << "checkpoint4" <<std::endl;
            tem_vertices.push_back(cross_result.y);
            tem_vertices.push_back(cross_result.z);
        }
    }
    std::copy(tem_vertices.begin(), tem_vertices.end(), vertices);
}
