
#include <glad/glad.h>  
#include <OpenGL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <inc/camera.h>

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../inc/my_texture.h"
#include "../inc/shader_m.h"
#include "tiny_obj_loader.h"



/*-----------------------------------------------------------------------*/
//Here are some mouse and keyboard function. You can change that.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float degreeX = (360 * lastX / 400);
float degreeY = (360 * lastY / 300);
bool firstMouse = true;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float OX = 0;//should be update to a new coordinate
float OY = 0;
float OZ = 0;
float currentFrame;

Camera *camera;

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


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void initPMV(Shader my_shader, Shader light_shader, glm::vec3 pointLightPositions[]) {
    camera = new Camera (glm::vec3(0.0f, 0.0f, 3.0f));

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    my_shader.use();
    my_shader.setMat4("projection", projection);
    my_shader.setVec3("viewPos", camera->Position);

    my_shader.setVec3("dirLight.direction", glm::vec3(1.01f, 1.01f, 1.01f));
    my_shader.setVec3("dirLight.ambient", glm::vec3(0.01f, 0.01f, 0.02f));
    my_shader.setVec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    my_shader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
    // point light 1
    my_shader.setVec3("pointLights[0].position", pointLightPositions[0]);
    my_shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    my_shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    my_shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    my_shader.setFloat("pointLights[0].constant", 1.0f);
    my_shader.setFloat("pointLights[0].linear", 0.09);
    my_shader.setFloat("pointLights[0].quadratic", 0.032);
    // point light 2
    my_shader.setVec3("pointLights[1].position", pointLightPositions[1]);
    my_shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    my_shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    my_shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    my_shader.setFloat("pointLights[1].constant", 1.0f);
    my_shader.setFloat("pointLights[1].linear", 0.09);
    my_shader.setFloat("pointLights[1].quadratic", 0.032);
    // point light 3
    my_shader.setVec3("pointLights[2].position", pointLightPositions[2]);
    my_shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    my_shader.setVec3("pointLights[2].diffuse", 0.6f, 0.1f, 0.8f);
    my_shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    my_shader.setFloat("pointLights[2].constant", 1.0f);
    my_shader.setFloat("pointLights[2].linear", 0.09);
    my_shader.setFloat("pointLights[2].quadratic", 0.032);
    // point light 4
    my_shader.setVec3("pointLights[3].position", pointLightPositions[3]);
    my_shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    my_shader.setVec3("pointLights[3].diffuse", 0.1f, 1.1f, 0.8f);
    my_shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    my_shader.setFloat("pointLights[3].constant", 1.0f);
    my_shader.setFloat("pointLights[3].linear", 0.09);
    my_shader.setFloat("pointLights[3].quadratic", 0.032);

    my_shader.setInt("texture1", 0);
    my_shader.setInt("texture2", 1);

    light_shader.use();
    glm::mat4 view = glm::mat4(1.0f);
    light_shader.setMat4("view", view);
    light_shader.setMat4("projection", projection);
    light_shader.setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0));
}

void changePMV(Shader shader, Shader light_shader) {
    glm::mat4 view;
    view = camera->GetViewMatrix();
    shader.use();
    // 更新shader中观察坐标系
    shader.setMat4("view", view);
    // shader更新视角位置
    shader.setVec3("viewPos", camera->Position);
    light_shader.use();
    light_shader.setMat4("view", view);
}
/*-----------------------------------------------------------------------*/

// calculate tangent space
glm::vec3 calculate_tangent(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3) {
    glm::vec3 edge1 = p2 - p1;
    glm::vec3 edge2 = p3 - p1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;
    glm::vec3 proTangent;

    GLfloat proF = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    proTangent.x = proF * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    proTangent.y = proF * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    proTangent.z = proF * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    proTangent = glm::normalize(proTangent);
    return proTangent;
}


//This an function to get v, vt and vn. 
void make_face(std::vector<float> v, std::vector<float> vt, std::vector<float> vn, std::vector<unsigned int> f,
	std::vector<glm::vec3>& points, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& tangent)
{
	if (f.size() % 3 != 0){
	} else {
        for (int i = 0; i < f.size()/3; i += 1)
        {
            // store one face
            std::vector<glm::vec3> proPoints;
            std::vector<glm::vec2> proUVs;
            int k = i * 3;
            for (int j = 0; j < 3; j++)
            {
                points.emplace_back(v[f[k + j] * 3], v[f[k + j] * 3 + 1], v[f[k + j] * 3 + 2]);
                proPoints.emplace_back(v[f[k + j] * 3], v[f[k + j] * 3 + 1], v[f[k + j] * 3 + 2]);
                normals.emplace_back(vn[f[k + j] * 3], vn[f[k + j] * 3 + 1], vn[f[k + j] * 3 + 2]);
                uvs.emplace_back(vt[f[k + j] * 2], vt[f[k + j] * 2 + 1]);
                proUVs.emplace_back(vt[f[k + j] * 2], vt[f[k + j] * 2 + 1]);
            }
            // use the data from one face to calculate the tangent space
            glm::vec3 proTangent = calculate_tangent(proPoints[0], proPoints[1], proPoints[2], proUVs[0], proUVs[1], proUVs[2]);
            // one tangent space for one point
            tangent.push_back(proTangent);
            tangent.push_back(proTangent);
            tangent.push_back(proTangent);
        }
	}
}

void get_vec3(std::vector<float> list, std::vector<glm::vec3> &vec)
{
	int n = list.size() / 3;
	for (int i = 0; i < n; i++)
	{
		vec.emplace_back(list[i * 3], list[i * 3 + 1], list[i * 3 + 2]);
	}
}
void get_vec2(std::vector<float> list, std::vector<glm::vec2>& vec)
{
	int n = list.size() / 2;
	for (int i = 0; i < n; i++)
	{
		vec.emplace_back(list[i * 2], list[i * 2 + 1]);
	}
}

void drawLights(Shader light_shader, glm::vec3 pointLightPositions[], unsigned int VAO) {
    light_shader.use();
    // 电光源
    for (int i = 0;i < 4;i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        light_shader.setMat4("model", model);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    // 模拟线光源
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model,  glm::vec3(-20.01f, -20.01f, -20.01f));
    model = glm::scale(model, glm::vec3(3.0f));
    light_shader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

// 返回当前文件夹位置绝对值
std::string GetCurrentWorkingDir() {
    char buff[FILENAME_MAX];
    GetCurrentDir( buff, FILENAME_MAX );
    std::string current_working_dir(buff);
    return current_working_dir;
}



int main()
{
    std::string root_dir = GetCurrentWorkingDir();
    int len = root_dir.length();
    // cmakeLists.txt所在文件目录绝对位置
    root_dir = root_dir.substr(0, len - 18);
    std::string model_dir = root_dir.substr(0, root_dir.length() - 18) + "model";

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Here you need to fill construct function of class Shader. And you need to understand other funtions in Shader.//
	// Then, write code in shader_m.vs, shader_m.fs and shader_m.gs to finish the tasks.                             //
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Shader my_shader(
            (root_dir + "/src/shader_m.vs").c_str(),
            (root_dir + "/src/shader_m.fs").c_str()
	);
	//A shader for light visiable source
	Shader lampShader((root_dir + "/src/lamp.vs").c_str(), (root_dir + "/src/lamp.fs").c_str());
	


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// tiny::LoadObj is a function to load obj file. The output is shape_t and material_t.                         //
	// "shape.mesh" is a mesh struct. "mesh.positions", "mesh.normals", "mesh.texcoords" corresponds to v,vn,vt.   //
	// For example:                                                                                                //
	// positions[0],positions[1],positions[2] -> v 0,0,1                                                           //
	// positions[3],positions[4],positions[5] -> v 0,1,0                                                           //
	// "mesh.indice" corresponds to f, but it is different from f. Each element is an index for all of v,vn,vt.    //
	// positions[0],positions[1],positions[2] -> v 0,0,1  positions[0],positions[1],positions[2] -> v 0,0,1        //
	// You can read tiny_obj_loader.h to get more specific information.                                            //
	//                                                                                                             //
	// I have write make_face for you.  It will return v, vt, vn in vec form (each element if for one point).      //
	// These informations can help you to do normal mapping.  (You can calculate tangent here)                     //
	// Since i did not assign uv for noraml map, you just need use vt as uv for normal map, but you will find it is//
	//  ugly. So please render a box to show a nice normal mapping. (Do normal mapping on obj and box)             //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string if_load_succeed = tinyobj::LoadObj(shapes, materials,
	        (model_dir + "/pikaqiu.obj").c_str()
	);
    if (!if_load_succeed.empty()) {
        std::cout<<"LOAD OBJ FILE ERROR: "<<if_load_succeed<<std::endl;
    }


	std::vector<unsigned int> obj_VBO_l, obj_VAO_l;
    for (int i = 0; i < shapes.size(); i++)
	{
		
		std::vector < glm::vec3 > out_vertices;
		std::vector < glm::vec2 > out_uvs;
		std::vector < glm::vec3 > out_normals;
        std::vector < glm::vec3 > out_tangent;

		// out_vertices, out_uvs, out_normals will get v, vt and vn.
        make_face(shapes[i].mesh.positions, shapes[i].mesh.texcoords, shapes[i].mesh.normals, shapes[i].mesh.indices,
                  out_vertices, out_normals, out_uvs, out_tangent);
        float vertices[out_vertices.size() * 11];
        for (int j = 0;j < out_vertices.size();j++) {
            vertices[j*11] = out_vertices[j].x;
            vertices[j*11 + 1] = out_vertices[j].y;
            vertices[j*11 + 2] = out_vertices[j].z;
            vertices[j*11 + 3] = out_normals[j].x;
            vertices[j*11 + 4] = out_normals[j].y;
            vertices[j*11 + 5] = out_normals[j].z;
            vertices[j*11 + 6] = out_uvs[j].x;
            vertices[j*11 + 7] = out_uvs[j].y;
            vertices[j*11 + 8] = out_tangent[j].x;
            vertices[j*11 + 9] = out_tangent[j].y;
            vertices[j*11 + 10] = out_tangent[j].z;
        }
        obj_VAO_l.push_back(1);
        obj_VBO_l.push_back(1);
        glGenVertexArrays(1, &obj_VAO_l[i]);
        glGenBuffers(1, &obj_VBO_l[i]);

        glBindVertexArray(obj_VAO_l[i]);
        glBindBuffer(GL_ARRAY_BUFFER, obj_VBO_l[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)nullptr);
        glEnableVertexAttribArray(0);
        // normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // texture coords
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        // tangent
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(3);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render a box to show nice normal mapping.                                                                   //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	float raw_vertices_cube_0[] = {

		// positions          // normals           // texture coords

		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f

	};

    float vertices_cube_0[36 * 11];
    for (int l = 0; l < 36; l += 3) {
        std::vector<glm::vec3> points;
        std::vector<glm::vec2> uvs;
        std::vector<float> cod;
        std::vector<float> uv;
        for (int j = 0; j < 3; ++j) {
            for (int i = 0; i < 3; ++i) {
                cod.push_back(raw_vertices_cube_0[(l + j) * 8 + i]);
                // std::cout<<"push point: "<<raw_vertices_cube_0[(l + j) * 8 + i];
            }
            // std::cout<<std::endl;
            for (int i = 0; i < 2; ++i) {
                uv.push_back(raw_vertices_cube_0[(l + j) * 8 + i + 6]);
                // std::cout<<"push uvs: "<<raw_vertices_cube_0[(l + j) * 8 + i + 6];
            }
            // std::cout<<std::endl;
        }
        get_vec3(cod, points);
        get_vec2(uv, uvs);
        glm::vec3 proTangent = calculate_tangent(points[0], points[1], points[2], uvs[0], uvs[1], uvs[2]);
        for (int i = 0; i < 8; ++i) {
            vertices_cube_0[l * 11 + i] = raw_vertices_cube_0[l * 8 + i];
            vertices_cube_0[(l + 1) * 11 + i] = raw_vertices_cube_0[(l + 1) * 8 + i];
            vertices_cube_0[(l + 2) * 11 + i] = raw_vertices_cube_0[(l + 2) * 8 + i];
        }
        for (int k = 0; k < 3; ++k) {
            vertices_cube_0[(l + k) * 11 + 8] = proTangent.x;
            vertices_cube_0[(l + k) * 11 + 9] = proTangent.y;
            vertices_cube_0[(l + k) * 11 + 10] = proTangent.z;
        }
    }
    /*
    std::cout<<"vertices:";
    for (int k = 0; k < 36 * 11; ++k) {
        if (k % 11 == 0) {
            std::cout<<std::endl;
        }
        std::cout<< vertices_cube_0[k]<<" ";
    }
     */

    unsigned int VBO, VAO;
    // the first argument is id
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_cube_0), vertices_cube_0, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coords
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// You need to fill this function which is defined in my_texture.h. The parameter is the path of your image.   //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	unsigned int texture1 = loadTexture((model_dir + "/brickwall.jpg").c_str());
    unsigned int texture2 = loadTexture((model_dir + "/normal_map.jpg").c_str());
    unsigned int texture3 = loadTexture((model_dir + "/p_r.jpg").c_str());
    unsigned int texture_light = loadTexture((model_dir + "/2.jpg").c_str());
    unsigned int texture_eye = loadTexture((model_dir + "/y.jpg").c_str());

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Here we defined pointlights in shader and passed some parameter for you. You can take this as an example.   //
	// Or you can change it if you like.                                                                           //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    glm::vec3 pointLightPositions[] = {
            glm::vec3(5.7f,  5.2f,  2.0f),
            glm::vec3(2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3(0.0f,  0.0f, -3.0f)
    };

    initPMV(my_shader, lampShader, pointLightPositions);

    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        changePMV(my_shader, lampShader);
		//Update Camera Matrix
		glFlush();
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glLightModeli(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//  Render an object using texture and normal map.                                                             //
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        my_shader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.5f, -1.5f, -1.5f));
        my_shader.setMat4("model", model);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//  Render the object in .obj file. You need to set materials and wrap texture for objects.                    //
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        model = glm::mat4(1.0f);
        my_shader.setMat4("model", model);
        glBindVertexArray(obj_VAO_l[2]);
        glDrawArrays(GL_TRIANGLES, 0, shapes[2].mesh.indices.size());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_eye);

        my_shader.use();
        model = glm::mat4(1.0f);
        my_shader.setMat4("model", model);
        glBindVertexArray(obj_VAO_l[0]);
        glDrawArrays(GL_TRIANGLES, 0, shapes[0].mesh.indices.size());

        my_shader.use();
        model = glm::mat4(1.0f);
        my_shader.setMat4("model", model);
        glBindVertexArray(obj_VAO_l[1]);
        glDrawArrays(GL_TRIANGLES, 0, shapes[1].mesh.indices.size());



		/////////////////////////////////////////////////////////////////////
		
		/////////////////////////////end/////////////////////////////////////
        // Visualize point lights and dir lights
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_light);
        drawLights(lampShader, pointLightPositions, VAO);

        glfwSwapBuffers(window);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    unsigned int arr[obj_VAO_l.size()];

    std::copy(obj_VAO_l.begin(), obj_VAO_l.end(), arr);
    glDeleteVertexArrays(obj_VAO_l.size(), arr);
    std::copy(obj_VBO_l.begin(), obj_VBO_l.end(), arr);
    glDeleteBuffers(obj_VBO_l.size(), arr);

    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    delete camera;
    return 0;
}

