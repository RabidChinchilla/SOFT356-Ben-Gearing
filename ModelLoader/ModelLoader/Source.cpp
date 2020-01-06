#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GLFW/glfw3.h"
#include "LoadShaders.h"
#include <glm/glm.hpp> //includes GLM
#include <glm/ext/matrix_transform.hpp> // GLM: translate, rotate
#include <glm/ext/matrix_clip_space.hpp> // GLM: perspective and ortho 
#include <glm/gtc/type_ptr.hpp> // GLM: access to the value_ptr
#include<sstream>
#include<string>
#include<fstream>
#include <iostream>
#include<vector>
#include<algorithm>

using namespace glm;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "LoadShaders.h"
using namespace std;

#include <GL/glut.h>

#define BUFFER_OFFSET(a) ((void*)(a))

string path; //String to hold user input for file address


//Vectors used in parsing which strings read are split into accordingly:
vector<vec3> vVector;
vector<vec2> vtVector;
vector<vec3> vnVector;

vector<string> tempSplit; //Passed into when parsing face values

enum VAO_IDs { Object, NumVAOs = 1 };
enum Buffer_IDs { Triangles, Colours, Normals, Textures, Indices, NumBuffers = 5 };
//enum Attrib_IDs { vPosition = 0, aTexCoord = 1, vNormals = 2 };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint texture1;

GLuint shader;

int numberOfVertices;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void loadModel(string fileToOpen, vector<GLfloat>& outVertices, vector<GLfloat>& outTextures, vector<GLfloat>& outNormals);
void loadMTLFile(string file_name, vector<GLfloat>& colour, vector<GLfloat>& diffuse, vector<GLfloat>& specular, GLfloat& ns, string& texture_name);

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
	glm::vec4 ambient = glm::vec4(colour[0], colour[1], colour[2], colour[3]);
	//adding the Uniform to the shader
	GLuint aLoc = glGetUniformLocation(shader, "ambient");
	glUniform4fv(aLoc, 1, glm::value_ptr(ambient));

	// light object
	glm::vec3 lightPos = glm::vec3(100.0f, 25.0f, 100.0f);
	GLuint dLightPosLoc = glGetUniformLocation(shader, "lightPos");
	glUniform3fv(dLightPosLoc, 1, glm::value_ptr(lightPos));


	// diffuse light
	glm::vec3 diffuseLight = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);
	GLuint dLightLoc = glGetUniformLocation(shader, "dLight");
	glUniform3fv(dLightLoc, 1, glm::value_ptr(diffuseLight));

	// specular light
	glm::vec3 specularLight = glm::vec3(specular[0], specular[1], specular[2]);
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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(Triangles, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


	//Colour Binding
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Colours]);
	glBufferStorage(GL_ARRAY_BUFFER, sizeof(colours), colours, 0);

	glVertexAttribPointer(Colours, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


	//Normal Binding
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Normals]);
	glBufferStorage(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), 0);


	glVertexAttribPointer(Normals, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//Texture Binding
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[Textures]);
	glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(GLfloat), textures.data(), GL_STATIC_DRAW);
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
	unsigned char* data = stbi_load(texture_name.c_str(), &width, &height, &nrChannels, 0);
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

	string fileToOpen;
	cout << "Please enter the Object you want to open: ";
	cin >> fileToOpen;
	cout << fileToOpen << ".obj is the file you're opening\n";

	while (true) // While loop for loading in file on name entry, Displaying, Allowing User Input
	{

		//cin >> fileToOpen;
		loadModel(fileToOpen, vertices, textures, normals);
		loadMTLFile(fileToOpen, colour, diffuse, specular, ns, texture_name);
		glfwInit();

		GLFWwindow* window = glfwCreateWindow(800, 600, "Shaded Cube", NULL, NULL); // Loading Window

		glfwMakeContextCurrent(window); //Making sure that can have User Input
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		// tell GLFW to capture our mouse
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glewInit();

		cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);	//Resetting Camera each time scene re-opens
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

		init(vertices, textures, normals, texture_name, colour, diffuse, specular, ns);
		GLfloat timer = 0.0f;
		while (!glfwWindowShouldClose(window))
		{

			processInput(window);

			display(timer);
			glfwSwapBuffers(window);
			glfwPollEvents();
			timer += 0.1f;


		}


		glfwDestroyWindow(window);

	}
}

void loadModel(string fileToOpen, vector<GLfloat>& outVertices, vector<GLfloat>& outTextures, vector<GLfloat>& outNormals) {

	//file parsing
	vector<GLfloat> vertices;
	vector<GLfloat> textures;
	vector<GLfloat> normals;
	vector<GLuint> vIndices, tIndices, nIndices;

	//path = "Media/Creeper-obj/" + fileToOpen + ".obj";
	path = "Media/" + fileToOpen + ".obj";

	string currentLine;
	ifstream file(path);


	if (file.is_open()) // Opening file
	{


		while (getline(file, currentLine)) // Reading through each line looking at values of string to seperate
		{
			std::cout << currentLine << std::endl;

			if (currentLine[0] == 'v' && currentLine[1] == ' ') //Seperating based on char V and char _
			{
				GLfloat fl;

				istringstream data(currentLine.substr(2));
				data >> fl;
				vertices.push_back(fl);
				data >> fl;
				vertices.push_back(fl);
				data >> fl;
				vertices.push_back(fl);

				//cout << vertices[0];

			}
			else

				if (currentLine[0] == 'v' && currentLine[1] == 't') //Seperating based on char V and Char T
				{

					GLfloat fl;

					istringstream data(currentLine.substr(2));
					data >> fl;
					textures.push_back(fl);
					data >> fl;
					textures.push_back(fl);


					//cout << vertices[0];

				}
				else

					if (currentLine[0] == 'v' && currentLine[1] == 'n') //Seperating based on char V and char N
					{

						GLfloat fl;

						istringstream data(currentLine.substr(2));
						data >> fl;
						normals.push_back(fl);
						data >> fl;
						normals.push_back(fl);
						data >> fl;
						normals.push_back(fl);

						//cout << vertices[0];

					}
					else

						if (currentLine[0] == 'f' && currentLine[1] == ' ') //Seperating based on char F and char _
						{

							replace(currentLine.begin(), currentLine.end(), '/', ' ');  //Replacing the / with _ to make seperating easier

							GLuint tempVertices, tempNormals, tempTextures;
							GLuint vertexIndex[4], textureIndex[4], normalIndex[4], num;
							istringstream data(currentLine.substr(2));

							data >> num;		//pushing vertex, textures, normals back first time
							vertexIndex[0] = num;

							data >> num;
							textureIndex[0] = num;

							data >> num;
							normalIndex[0] = num;


							data >> num;		//pushing vertex, textures, normals back second time
							vertexIndex[1] = num;
							data >> num;
							textureIndex[1] = num;
							data >> num;
							normalIndex[1] = num;

							data >> num;		//pushing vertex, textures, normals back third time
							vertexIndex[2] = num;

							data >> num;
							textureIndex[2] = num;

							data >> num;
							normalIndex[2] = num;


							data >> num;		//pushing vertex, textures, normals back fourth time
							vertexIndex[3] = num;
							data >> num;
							textureIndex[3] = num;
							data >> num;
							normalIndex[3] = num;

							//cout << vertexIndex[0] << "/" << textureIndex[0] << "/" << normalIndex[0] << endl;	//printing values to make sure everything working correctly, commented out after making sure.
							//cout << vertexIndex[1] << "/" << textureIndex[1] << "/" << normalIndex[1] << endl;
							//cout << vertexIndex[2] << "/" << textureIndex[2] << "/" << normalIndex[2] << endl;
							//cout << vertexIndex[3] << "/" << textureIndex[3] << "/" << normalIndex[3] << endl;*/


							vIndices.push_back(vertexIndex[0]);	//
							tIndices.push_back(textureIndex[0]);
							nIndices.push_back(normalIndex[0]);

							vIndices.push_back(vertexIndex[1]);
							tIndices.push_back(textureIndex[1]);
							nIndices.push_back(normalIndex[1]);

							vIndices.push_back(vertexIndex[2]);
							tIndices.push_back(textureIndex[2]);
							nIndices.push_back(normalIndex[2]);

							vIndices.push_back(vertexIndex[0]);
							tIndices.push_back(textureIndex[0]);
							nIndices.push_back(normalIndex[0]);

							vIndices.push_back(vertexIndex[2]);
							tIndices.push_back(textureIndex[2]);
							nIndices.push_back(normalIndex[2]);

							vIndices.push_back(vertexIndex[3]);
							tIndices.push_back(textureIndex[3]);
							nIndices.push_back(normalIndex[3]);





							//cout << vertexIndex[0] << "/" << textureIndex[0] << "/" << normalIndex[0] << endl;

						}

		}

	}

	for (GLint i = 0; i < vIndices.size(); i++) {

		GLint vertexIndex = (vIndices[i] - 1) * 3;
		GLint normalIndex = (nIndices[i] - 1) * 3;


		outVertices.push_back(vertices[vertexIndex]);
		outVertices.push_back(vertices[vertexIndex + 1]);
		outVertices.push_back(vertices[vertexIndex + 2]);

		outNormals.push_back(normals[normalIndex]);
		outNormals.push_back(normals[normalIndex + 1]);
		outNormals.push_back(normals[normalIndex + 2]);


	}

	for (GLint i = 0; i < tIndices.size(); i++) {		//For loop for pushing textures back as did not work for some reason in the loop above

		GLint textureIndex = (tIndices[i] - 1) * 2;

		outTextures.push_back(textures[textureIndex]);
		outTextures.push_back(textures[textureIndex + 1]);

	}
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // Close on exit
		glfwSetWindowShouldClose(window, true);


	float cameraSpeed = 2.5 * deltaTime; // Setting Camera Speed
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) //Allowing for line mode on holding key down
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

void
loadMTLFile(string file_name, vector<GLfloat>& colour, vector<GLfloat>& diffuse, vector<GLfloat>& specular, GLfloat& ns, string& texture_name) //Loading Material file
{

	string line;
	ifstream myFile("Media/Creeper-obj/" + file_name + ".mtl"); // Reading in specific file
	GLfloat transparency = -1;


	if (myFile.is_open()) // Opening file
	{
		GLfloat fl;


		while (getline(myFile, line)) // Reading through each line looking at values of string to seperate
		{
			std::cout << line << std::endl;

			if (line[0] == 'K' && line[1] == 'a') 
			{

				istringstream data(line.substr(2));
				data >> fl;
				colour.push_back(fl);

				data >> fl;
				colour.push_back(fl);

				data >> fl;
				colour.push_back(fl);

				data >> fl;
				colour.push_back(fl);


			}
			else

				if (line[0] == 'd' && line[1] == ' ') 
				{

					istringstream data(line.substr(2));
					data >> fl;
					transparency = fl;

					if (transparency == -1)
					{
						transparency = 1;
					}

					colour.push_back(transparency);

				}
				else

					if (line[0] == 'K' && line[1] == 'd') 
					{


						istringstream data(line.substr(2));
						data >> fl;
						diffuse.push_back(fl);

						data >> fl;
						diffuse.push_back(fl);

						data >> fl;
						diffuse.push_back(fl);
					}
					else

						if (line[0] == 'K' && line[1] == 's') 
						{

							istringstream data(line.substr(2));
							data >> fl;
							specular.push_back(fl);

							data >> fl;
							specular.push_back(fl);

							data >> fl;
							specular.push_back(fl);


						}
						else

							if (line[0] == 'N' && line[1] == 's') 
							{


								istringstream data(line.substr(2));
								data >> fl;
								ns = fl;

							}
							else

								if (line.substr(0, 6) == "map_Kd")
								{

									istringstream data(line.substr(7));
									texture_name = data.str();


								}

		}

		texture_name = "Media/" + texture_name; //Name of texture


	}

}
