#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <istream>
#include <sstream>
#include <vector>
#include <cstdio>
#include <regex>

// OpenGL Related Libraries
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GLFW/glfw3.h>

// GLM Related Libraries
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "LoadShaders.h"
using namespace std;

#include <GL/glut.h>

#define BUFFER_OFFSET(a) ((void*)(a))

//Struct containing vectors which parsed values are passed into by v/vt/vnVector:
struct fValues {
	vector<vec3> fVertexValueInd;
	vector<vec2> fVertexTextureInd;
	vector<vec3> fVertexNormalInd;
};

string path; //String to hold user input for file address

fValues Value; //fValues object

//Vectors used in parsing which strings read are split into accordingly:
vector<vec3> vVector;
vector<vec2> vtVector;
vector<vec3> vnVector;

vector<string> tempSplit; //Passed into when parsing face values

enum VAO_IDs { Object, NumVAOs = 1 };
enum Buffer_IDs { Triangles, Colours, Normals, Textures, Indices, NumBuffers = 5 };
enum Attrib_IDs { vPosition = 0, aTexCoord = 1, vNormals = 2 };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint texture1;
GLuint shader;

int numberOfVertices;

//ADDED AS PART OF CW2:
GLfloat rotateVel; //Used to define object rotation velocity
GLfloat objScale = 1.0f; //Used to define scale of rendered object

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

#define BUFFER_OFFSET(a) ((void*)(a))

void init(vector<GLfloat>& vertices, vector<GLfloat>& textures, vector<GLfloat>& normals, string texture_name, vector<GLfloat>& colour, vector<GLfloat>& diffuse, vector<GLfloat>& specular, GLfloat& ns)
{
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Object]);

	ShaderInfo shaders[] =
	{
		{ GL_VERTEX_SHADER, "Media/triangles.vert"},
		{ GL_FRAGMENT_SHADER, "Media/triangles.frag"},
		{ GL_NONE, NULL}
	};

	shader = LoadShaders(shaders);
	glUseProgram(shader);

	//Lighting
	// ambient light
	glm::vec4 ambient = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	//adding the Uniform to the shader
	GLuint aLoc = glGetUniformLocation(shader, "ambient");
	glUniform4fv(aLoc, 1, glm::value_ptr(ambient));

	// light object
	glm::vec3 lightPos = glm::vec3(100.0f, 25.0f, 100.0f);
	GLuint dLightPosLoc = glGetUniformLocation(shader, "lightPos");
	glUniform3fv(dLightPosLoc, 1, glm::value_ptr(lightPos));


	// diffuse light
	glm::vec3 diffuseLight = glm::vec3(0.5f, 0.2f, 0.7f);
	GLuint dLightLoc = glGetUniformLocation(shader, "dLight");
	glUniform3fv(dLightLoc, 1, glm::value_ptr(diffuseLight));

	// specular light
	glm::vec3 specularLight = glm::vec3(0.7f);
	GLfloat shininess = 256; //128 is max value
	GLuint sLightLoc = glGetUniformLocation(shader, "sLight");
	GLuint sShineLoc = glGetUniformLocation(shader, "sShine");
	glUniform3fv(sLightLoc, 1, glm::value_ptr(specularLight));
	glUniform1fv(sShineLoc, 1, &shininess);

	GLfloat  colours[][4] = {
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },

		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },

		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },

		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },

		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },

		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f },

	};

	//string path;

	////file parsing
	//vector<glm::vec3> vertices;
	//vector<glm::vec2> textures;
	//vector<glm::vec3> normals;
	//vector<glm::vec3> tempVertices, tempNormals;
	//vector<glm::vec2> tempTextures;
	//vector<int> vIndices, tIndices, nIndices, indices;

	//path = "Media/Creeper-obj/" + fileToOpen + ".obj";

	//string currentLine;
	//ifstream file(path);

	//if (file.is_open()) {
	//	cout << "File open";
	//	while (getline(file, currentLine)) {
	//		if (currentLine.at(0) == 'v') {
	//			char prefixOfValues[3]; //gets the letters that are in front of the numbers in the file
	//			glm::vec3 vertex;
	//			sscanf_s(currentLine.c_str(), "%s %f %f %f\n", prefixOfValues, sizeof(prefixOfValues), &vertex.x, &vertex.y, &vertex.z);

	//			if (strcmp(prefixOfValues, "v") == 0) {

	//				tempVertices.push_back(vertex);
	//			}
	//			else if (strcmp(prefixOfValues, "vt") == 0) {
	//				tempTextures.push_back(vertex);
	//			}
	//			else if (strcmp(prefixOfValues, "vn") == 0) {
	//				tempNormals.push_back(vertex);
	//			}
	//		}
	//		else if (currentLine.at(0) == 'f') {
	//			int verticesIndex[4], textureIndex[4], normalIndex[4];
	//			int matches = sscanf_s(currentLine.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &verticesIndex[0], &textureIndex[0], &normalIndex[0], &verticesIndex[1], &textureIndex[1], &normalIndex[1], &verticesIndex[2], &textureIndex[2], &normalIndex[2], &verticesIndex[3], &textureIndex[3], &normalIndex[3]);

	//			if (matches == 9) {
	//				for (int i = 0; i < 3; i++) {
	//					vIndices.push_back(verticesIndex[i]);
	//					tIndices.push_back(textureIndex[i]);
	//					nIndices.push_back(normalIndex[i]);
	//				}
	//			}
	//			else if (matches == 12) {
	//				for (int i = 0; i < 6; i++) {
	//					//convert squares to triangles
	//					if (i < 4) {
	//						vIndices.push_back(verticesIndex[i]);
	//						tIndices.push_back(textureIndex[i]);
	//						nIndices.push_back(normalIndex[i]);
	//					}
	//					else if (i == 4) {
	//						vIndices.push_back(verticesIndex[0]);
	//						tIndices.push_back(textureIndex[0]);
	//						nIndices.push_back(normalIndex[0]);
	//					}
	//					else {
	//						vIndices.push_back(verticesIndex[2]);
	//						tIndices.push_back(textureIndex[2]);
	//						nIndices.push_back(normalIndex[2]);
	//					}
	//				}
	//			}
	//			else {
	//				cout << "File doesn't match the format";
	//			}
	//		}

	//	}
	//	//associating face data with vertex data
	//	for (int i = 0; i < vIndices.size(); i++) {
	//		int vertexIndex = vIndices[i];
	//		glm::vec3 vertex = tempVertices[vertexIndex - 1];
	//		vertices.push_back(vertex);
	//		indices.push_back(i);
	//	}
	//	for (int i = 0; i < tIndices.size(); i++) {
	//		int textureIndex = tIndices[i];
	//		glm::vec2 texture = tempTextures[textureIndex - 1];
	//		textures.push_back(texture);
	//	}
	//	for (int i = 0; i < nIndices.size(); i++) {
	//		int normalIndex = nIndices[i];
	//		glm::vec3 normal = tempNormals[normalIndex - 1];
	//		normals.push_back(normal);
	//	}
	//	numberOfVertices = vertices.size();
	//	file.close();
	//}
	//else {
	//	cout << "File not found";
	//}

	//feeding VAO's into buffers

	glGenBuffers(NumBuffers, Buffers);

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Triangles]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(Triangles, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[Indices]);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);




	glVertexAttribPointer(Triangles, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//Colour Binding
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Colours]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(colours), colours, 0);

	glVertexAttribPointer(Colours, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


	//Normal Binding
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Normals]);
	glBufferStorage(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], 0);


	glVertexAttribPointer(Normals, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//Texture Binding
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Textures]);
	glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(glm::vec2), &textures[0], GL_STATIC_DRAW);
	glVertexAttribPointer(Textures, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// load and create a texture 
	// -------------------------

	// texture 1
	// ---------
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	GLint width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load("Media/Creeper-obj/Texture.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);


	glUniform1i(glGetUniformLocation(shader, "texture1"), 0);


	// creating the model matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));


	// creating the view matrix
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));

	// creating the projection matrix
	glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3, 0.1f, 20.0f);

	// Adding all matrices up to create combined matrix
	glm::mat4 mv = view * model;


	//adding the Uniform to the shader
	int mvLoc = glGetUniformLocation(shader, "mv_matrix");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mv));
	//adding the Uniform to the shader
	int pLoc = glGetUniformLocation(shader, "p_matrix");
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));





	glEnableVertexAttribArray(Triangles);
	glEnableVertexAttribArray(Colours);
	glEnableVertexAttribArray(Textures);
	glEnableVertexAttribArray(Normals);
}

void display(GLfloat delta)
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	glClearBufferfv(GL_COLOR, 0, black);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind textures on corresponding texture units
	//glFrontFace(GL_CW);
	//glCullFace(GL_BACK); //commented out because it caused the model to be see through
	glEnable(GL_CULL_FACE);
	// creating the model matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
	model = glm::rotate(model, glm::radians(delta), glm::vec3(0.5f, 0.2f, 0.3f));


	// creating the view matrix
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f)); // 1st changes horizontal position, 2nd changes vertical and 3rd changes z position

	// creating the projection matrix
	//glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3, 0.1f, 20.0f);
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	

	// Adding all matrices up to create combined matrix
	glm::mat4 mv = view * model;


	//adding the Uniform to the shader
	int mvLoc = glGetUniformLocation(shader, "mv_matrix");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mv));
	//adding the Uniform to the shader
	int pLoc = glGetUniformLocation(shader, "p_matrix");
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));




	glBindVertexArray(VAOs[Object]);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glDrawElements(GL_TRIANGLES, numberOfVertices, GL_UNSIGNED_INT, 0); //Draw first Shape

	model = glm::rotate(model, glm::radians(delta), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));
	projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	mv = view * model;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mv));
	glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glDrawElements(GL_TRIANGLES, numberOfVertices, GL_UNSIGNED_INT, 0);//Draw second Shape
}

int main(int argc, char** argv)
{
	vector<GLfloat>vertices;
	vector<GLfloat>textures;
	vector<GLfloat>normals;
	vector<GLfloat> colour; //Ambient
	GLfloat transparency = -1;
	vector<GLfloat> diffuse;
	vector<GLfloat> specular;
	GLfloat ns; //Specular Exponent
	string texture_name;
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(800, 600, "Model Loader", NULL, NULL); //orignal values were 800, 600

	glfwMakeContextCurrent(window);
	glewInit();

	string fileToOpen;
	cout << "Please enter the Object you want to open: ";
	cin >> fileToOpen;
	cout << fileToOpen << ".obj is the file you're opening\n";

	init(vertices, textures, normals, texture_name, colour, diffuse, specular, ns);
	GLfloat timer = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// uncomment to draw only wireframe 
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		display(timer);
		glfwSwapBuffers(window);
		glfwPollEvents();
		timer += 0.1f;
		processInput(window);
		// camera/view transformation
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);


	}

	glfwDestroyWindow(window);

	glfwTerminate();
}

void loadModel(string fileToOpen) {
	//string path;

	//file parsing
	vector<glm::vec3> vertices;
	vector<glm::vec2> textures;
	vector<glm::vec3> normals;
	vector<glm::vec3> tempVertices, tempNormals;
	vector<glm::vec2> tempTextures;
	vector<int> vIndices, tIndices, nIndices, indices;

	path = "Media/Creeper-obj/" + fileToOpen + ".obj";

	string currentLine;
	ifstream file(path);

	if (file.is_open()) {
		cout << "File open";
		while (getline(file, currentLine)) {
			if (currentLine.at(0) == 'v') {
				char prefixOfValues[3]; //gets the letters that are in front of the numbers in the file
				glm::vec3 vertex;
				sscanf_s(currentLine.c_str(), "%s %f %f %f\n", prefixOfValues, sizeof(prefixOfValues), &vertex.x, &vertex.y, &vertex.z);

				if (strcmp(prefixOfValues, "v") == 0) {

					tempVertices.push_back(vertex);
				}
				else if (strcmp(prefixOfValues, "vt") == 0) {
					tempTextures.push_back(vertex);
				}
				else if (strcmp(prefixOfValues, "vn") == 0) {
					tempNormals.push_back(vertex);
				}
			}
			else if (currentLine.at(0) == 'f') {
				int verticesIndex[4], textureIndex[4], normalIndex[4];
				int matches = sscanf_s(currentLine.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &verticesIndex[0], &textureIndex[0], &normalIndex[0], &verticesIndex[1], &textureIndex[1], &normalIndex[1], &verticesIndex[2], &textureIndex[2], &normalIndex[2], &verticesIndex[3], &textureIndex[3], &normalIndex[3]);

				if (matches == 9) {
					for (int i = 0; i < 3; i++) {
						vIndices.push_back(verticesIndex[i]);
						tIndices.push_back(textureIndex[i]);
						nIndices.push_back(normalIndex[i]);
					}
				}
				else if (matches == 12) {
					for (int i = 0; i < 6; i++) {
						//convert squares to triangles
						if (i < 4) {
							vIndices.push_back(verticesIndex[i]);
							tIndices.push_back(textureIndex[i]);
							nIndices.push_back(normalIndex[i]);
						}
						else if (i == 4) {
							vIndices.push_back(verticesIndex[0]);
							tIndices.push_back(textureIndex[0]);
							nIndices.push_back(normalIndex[0]);
						}
						else {
							vIndices.push_back(verticesIndex[2]);
							tIndices.push_back(textureIndex[2]);
							nIndices.push_back(normalIndex[2]);
						}
					}
				}
				else {
					cout << "File doesn't match the format";
				}
			}

		}
		//associating face data with vertex data
		for (int i = 0; i < vIndices.size(); i++) {
			int vertexIndex = vIndices[i];
			glm::vec3 vertex = tempVertices[vertexIndex - 1];
			vertices.push_back(vertex);
			indices.push_back(i);
		}
		for (int i = 0; i < tIndices.size(); i++) {
			int textureIndex = tIndices[i];
			glm::vec2 texture = tempTextures[textureIndex - 1];
			textures.push_back(texture);
		}
		for (int i = 0; i < nIndices.size(); i++) {
			int normalIndex = nIndices[i];
			glm::vec3 normal = tempNormals[normalIndex - 1];
			normals.push_back(normal);
		}
		numberOfVertices = vertices.size();
		file.close();
	}
	else {
		cout << "File not found";
	}
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
