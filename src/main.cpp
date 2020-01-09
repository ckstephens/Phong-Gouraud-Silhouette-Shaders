#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "Program.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define NUM_LIGHTS 2
#define NUM_MATERIALS 3
#define NUM_SHADERS 3

// A function clamping the input values to the lower and higher bounds
#define CLAMP(in, low, high) ((in) < (low) ? (low) : ((in) > (high) ? (high) : in))

GLFWwindow *window;

Program program[NUM_SHADERS];
std::vector<float> posBuff;
std::vector<float> norBuff;
std::vector<float> texBuff;

glm::vec3 eye(0.0f, 0.0f, 4.0f);

struct materialStruct {
	glm::vec3 ka, kd, ks;
	float s;
} materials[NUM_MATERIALS];

struct lightStruct {
	glm::vec3 position;
	glm::vec3 color;
} lights[NUM_LIGHTS];

int material, light, shader = 0;

struct spotLightStruct {
	bool enabled;
	glm::mat4 viewMatrix;
	glm::vec3 position;
	glm::vec3 direction;
	float angle;
	float power;
} spotLight;

void printLight() {
	std::cout << "Light: " << light + 1 << std::endl;
}

void printMaterial() {
	std::cout << "Material: " << material + 1 << std::endl;
}

void printShader() {
	std::string text;
	switch (shader) {
	case 0:
		text = "Gouraud";
		break;
	case 1:
		text = "Phong";
		break;
	case 2:
		text = "Silhouette";
		break;
	}
	std::cout << "Shader: " << text << std::endl;
}

void printSpotLight() {
	std::cout << "SpotLight toggled: " << spotLight.enabled << std::endl;
}

void InitWorldLights() {
	lights[0].position = glm::vec3(0.0f, 0.0f, 3.0f);
	lights[0].color = glm::vec3(0.5f, 0.5f, 0.5f);
	lights[1].position = glm::vec3(0.0f, 3.0f, 0.0f);
	lights[1].color = glm::vec3(0.2f, 0.2f, 0.2f);
	spotLight.position = glm::vec3(0.0f, 0.0f, 3.0f);
	spotLight.direction = glm::vec3(0.0f, 0.0f, 0.0f);
	spotLight.angle = 12.0f;
	spotLight.power = 30.0f;
}

void InitWorldMaterials() {
	materials[0].ka = glm::vec3(0.2f, 0.2f, 0.2f);
	materials[0].kd = glm::vec3(0.8f, 0.7f, 0.7f);
	materials[0].ks = glm::vec3(1.0f, 1.0f, 1.0f);
	materials[0].s = 10.0f;
	materials[1].ka = glm::vec3(0.0f, 0.2f, 0.2f);
	materials[1].kd = glm::vec3(0.5f, 0.7f, 0.2f);
	materials[1].ks = glm::vec3(0.1f, 1.0f, 0.1f);
	materials[1].s = 100.0f;
	materials[2].ka = glm::vec3(0.2f, 0.2f, 0.2f);
	materials[2].kd = glm::vec3(0.1f, 0.3f, 0.9f);
	materials[2].ks = glm::vec3(0.1f, 0.1f, 0.1f);
	materials[2].s = 1.0f;
}

void Display()
{
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), float(width) / float(height), 0.1f, 100.0f);
	glm::mat4 viewMatrix = glm::lookAt(eye, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 modelMatrix(1.0f);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, -1.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	program[shader].Bind();
	program[shader].SendUniformData(modelMatrix, "model");
	program[shader].SendUniformData(viewMatrix, "view");
	program[shader].SendUniformData(projectionMatrix, "projection");
	program[shader].SendUniformData(glm::inverse(glm::transpose(modelMatrix)), "model_ti");
	program[shader].SendUniformData(glm::inverse(glm::transpose(viewMatrix)), "view_ti");
	program[shader].SendUniformData(eye, "eye");
	program[shader].SendUniformData(materials[material].ka, "ka");
	program[shader].SendUniformData(materials[material].kd, "kd");
	program[shader].SendUniformData(materials[material].ks, "ks");
	program[shader].SendUniformData(materials[material].s, "s");
	program[shader].SendUniformData(spotLight.enabled, "spotLight.enabled");
	program[shader].SendUniformData(spotLight.position, "spotLight.position");
	program[shader].SendUniformData(spotLight.direction, "spotLight.direction");
	program[shader].SendUniformData(spotLight.power, "spotLight.power");
	program[shader].SendUniformData(spotLight.angle, "spotLight.angle");
	program[shader].SendUniformData(spotLight.viewMatrix, "spotLight.viewMatrix");
	for (int i = 0; i < NUM_LIGHTS; i++) {
		program[shader].SendUniformData(lights[i].position, std::string("lights[" + std::to_string(i) + "].position").c_str());
		program[shader].SendUniformData(lights[i].color, std::string("lights[" + std::to_string(i) + "].color").c_str());
	}
	glDrawArrays(GL_TRIANGLES, 0, posBuff.size() / 3);
	program[shader].Unbind();

}

// Keyboard character callback function
void CharacterCallback(GLFWwindow* lWindow, unsigned int key)
{
	switch (key) 
	{
	case 'q':
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		break;
	case 'M':
		material = (--material < 0) ? 0 : material;
		printMaterial();
		break;
	case 'm':
		material = (++material >= NUM_MATERIALS) ? NUM_MATERIALS-1 : material;
		printMaterial();
		break;
	case 'L':
		light = (--light < 0) ? 0 : light;
		printLight();
		break;
	case 'l':
		light = (++light >= NUM_LIGHTS) ? NUM_LIGHTS - 1 : light;
		printLight();
		break;
	case 'x':
			lights[light].position[0] += 1.0f;
		break;
	case 'X':
			lights[light].position[0] -= 1.0f;
		break;
	case 'y':
			lights[light].position[1] += 1.0f;
		break;
	case 'Y':
			lights[light].position[1] -= 1.0f;
		break;
	case 'z':
			lights[light].position[2] += 1.0f;
		break;
	case 'Z':
			lights[light].position[2] -= 1.0f;
		break;
	case '1':
		shader = 0;
		printShader();
		break;
	case '2':
		shader = 1;
		printShader();
		break;
	case '3':
		shader = 2;
		printShader();
		break;
	case 's':
		spotLight.enabled = !spotLight.enabled;
		printSpotLight();
		break;
	case 't':
		spotLight.power += 1.0f;
		break;
	case 'T':
		spotLight.power -= 1.0f;
		break;
	case 'a':
		spotLight.angle += 1.0f;
		break;
	case 'A':
		spotLight.angle -= 1.0f;
		break;
	case 'r':
		InitWorldLights();
		break;
	default:
		break;
	}
}


void FrameBufferSizeCallback(GLFWwindow* lWindow, int width, int height)
{
	glViewport(0, 0, width, height);
}

void LoadModel(char* name)
{
	// Taken from Shinjiro Sueda with slight modification
	std::string meshName(name);
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	if (!rc) {
		std::cerr << errStr << std::endl;
	}
	else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
					posBuff.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
					if (!attrib.normals.empty()) {
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 0]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 1]);
						norBuff.push_back(attrib.normals[3 * idx.normal_index + 2]);
					}
					if (!attrib.texcoords.empty()) {
						texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
						texBuff.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
}

void Init()
{
	glfwInit();
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Assignment4 - Caleb Stephens", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;
	glewInit();
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetCharCallback(window, CharacterCallback);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	LoadModel("../obj/bunny.obj");
	
	InitWorldLights();
	InitWorldMaterials();

	program[0].SetShadersFileName("../shaders/shader1.vert", "../shaders/shader1.frag");
	program[0].Init();
	program[0].SendAttributeData(posBuff, "vPositionModel");
	program[0].SendAttributeData(norBuff, "vNormalModel");
	program[1].SetShadersFileName("../shaders/shader2.vert", "../shaders/shader2.frag");
	program[1].Init();
	program[1].SendAttributeData(posBuff, "vPositionModel");
	program[1].SendAttributeData(norBuff, "vNormalModel");
	program[2].SetShadersFileName("../shaders/shader3.vert", "../shaders/shader3.frag");
	program[2].Init();
	program[2].SendAttributeData(posBuff, "vPositionModel");
	program[2].SendAttributeData(norBuff, "vNormalModel");


	printShader();
	printLight();
	printMaterial();
}


int main()
{	
	Init();
	while ( glfwWindowShouldClose(window) == 0) 
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Display();
		glFlush();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}