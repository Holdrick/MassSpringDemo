#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include "MassSpring.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

GLuint VAO;

GLuint shaderProgram;

MassSpring * massSpring;
MassSpring * pendulum;
MassSpring * cube;
MassSpring * cloth;

GOBJ * plane;

string vsFilename = "vert-phong.vs.glsl";
string fsFilename = "frag-phong.fs.glsl";

// Transformation matrices
mat4 rotMatrix = mat4(1.0f);
mat4 scaleMatrix = mat4(1.0f);
mat4 transMatrix = mat4(1.0f);
float scalar = 1.0f;

int width = 1200;
int height = 900;
static GLubyte shaderText[8192];

GLuint mv_ptr, proj_ptr, amb_ptr;

GLboolean isObj = false;

#define VERTEX_DATA 0
#define VERTEX_NORMAL 1

bool loadShaderFile(const char *filename, GLuint shader) {
	GLint shaderLength = 0;
	FILE *fp;

	fp = fopen(filename, "r");
	if (fp != NULL) {
		while (fgetc(fp) != EOF)
			shaderLength++;

		rewind(fp);
		fread(shaderText, 1, shaderLength, fp);
		shaderText[shaderLength] = '\0';

		fclose(fp);
	}
	else {
		return false;
	}

	GLchar* fsStringPtr[1];
	fsStringPtr[0] = (GLchar *)((const char*)shaderText);
	glShaderSource(shader, 1, (const GLchar **)fsStringPtr, NULL);

	return true;
}

void render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgram);

	scaleMatrix = scale(mat4(1.0f), vec3(scalar)) * scaleMatrix;

	//Model View Matrix setup
	mat4 modelView = lookAt(vec3(0.0f, 1.0f, -30.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	modelView *= rotMatrix * scaleMatrix * transMatrix;
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mv_matrix"), 1, GL_FALSE, value_ptr(modelView));

	//Projection Matrix setup
	mat4 proj = perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj_matrix"), 1, GL_FALSE, value_ptr(proj));

	glUniform3f(glGetUniformLocation(shaderProgram, "light_pos"), 50.0f, 50.0f, 50.0f);
	
	glUniform3f(glGetUniformLocation(shaderProgram, "ambient"), 1.0f, 0.0f, 1.0f);
	if (!isObj){
		massSpring->renderMS();
	}
	else{
		glUniform1f(glGetUniformLocation(shaderProgram, "specular_power"), 128.0f);
		glUniform3f(glGetUniformLocation(shaderProgram, "specular_albedo"), 0.1f, 0.1f, 0.1f);
		glUniform3f(glGetUniformLocation(shaderProgram, "diffuse_albedo"), 0.1f, 0.1f, 0.1f);
		massSpring->renderObj(amb_ptr);

		if (massSpring->ambient == BLUE){
			glUniform1f(glGetUniformLocation(shaderProgram, "specular_power"), 128.0f);
			glUniform3f(glGetUniformLocation(shaderProgram, "specular_albedo"), 0.1f, 0.1f, 0.1f);
			glUniform3f(glGetUniformLocation(shaderProgram, "diffuse_albedo"), 0.3f, 0.3f, 0.3f);
			glUniform3f(glGetUniformLocation(shaderProgram, "ambient"), 0.001f, 0.001f, 0.001f);
			plane->buffer();
			plane->draw(modelView, vec3(0.0f, 0.4f, 0.0f), mv_ptr);
		}
	}
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
		rotMatrix = rotate(mat4(1.0f), 0.5f, vec3(1.0f, 0.0f, 0.0f)) * rotMatrix;
	if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
		rotMatrix = rotate(mat4(1.0f), 0.5f, vec3(-1.0f, 0.0f, 0.0f)) * rotMatrix;

	if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		rotMatrix = rotate(mat4(1.0f), 0.5f, vec3(0.0f, 1.0f, 0.0f)) * rotMatrix;
	if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		rotMatrix = rotate(mat4(1.0f), 0.5f, vec3(0.0f, -1.0f, 0.0f)) * rotMatrix;
}


static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		scalar = 1.1f;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		scalar = 1.0f;

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		scalar = 1.0f / 1.1f;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		scalar = 1.0f;
}

void setupRenderingContext() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	GLuint hVertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint hFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	shaderProgram = (GLuint)NULL;
	GLint testVal;

	if (!loadShaderFile(vsFilename.c_str(), hVertexShader)) {
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		cout << "The shader " << vsFilename << " could not be found." << endl;
	}

	if (!loadShaderFile(fsFilename.c_str(), hFragmentShader)) {
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
		cout << "The shader " << fsFilename << " could not be found." << endl;
	}

	glCompileShader(hVertexShader);
	glCompileShader(hFragmentShader);

	// Check for any error generated during shader compilation
	glGetShaderiv(hVertexShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(hVertexShader, 8192, NULL, source);
		glGetShaderInfoLog(hVertexShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
	}
	glGetShaderiv(hFragmentShader, GL_COMPILE_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char source[8192];
		char infoLog[8192];
		glGetShaderSource(hFragmentShader, 8192, NULL, source);
		glGetShaderInfoLog(hFragmentShader, 8192, NULL, infoLog);
		cout << "The shader: " << endl << (const char*)source << endl << " failed to compile:" << endl;
		fprintf(stderr, "%s\n", infoLog);
		glDeleteShader(hVertexShader);
		glDeleteShader(hFragmentShader);
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, hVertexShader);
	glAttachShader(shaderProgram, hFragmentShader);

	glBindAttribLocation(shaderProgram, VERTEX_DATA, "position");
	glBindAttribLocation(shaderProgram, VERTEX_NORMAL, "normal");

	glLinkProgram(shaderProgram);
	glDeleteShader(hVertexShader);
	glDeleteShader(hFragmentShader);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &testVal);
	if (testVal == GL_FALSE) {
		char infoLog[1024];
		glGetProgramInfoLog(shaderProgram, 1024, NULL, infoLog);
		cout << "The shader program (" << vsFilename << " + " << fsFilename << ") failed to link:" << endl << (const char*)infoLog << endl;
		glDeleteProgram(shaderProgram);
		shaderProgram = (GLuint)NULL;
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	mv_ptr = glGetUniformLocation(shaderProgram, "mv_matrix");
	proj_ptr = glGetUniformLocation(shaderProgram, "prog_matrix");
	amb_ptr = glGetUniformLocation(shaderProgram, "ambient");
}

int main(int argc, char **argv)
{
	GLFWwindow* window;

	if (argc < 2){
		cout << "Please enter a spring system (spring, pendulum, cube, or cloth)" << endl;
		return -1;
	}
	
	if (strcmp(argv[1], "spring") == 0){
		if (argc != 3){
			cout << "Enter mass of object\n";
			return -1;
		}
		GLfloat ballMass = (GLfloat)atof(argv[2]);
		massSpring = new MassSpring(ballMass, SPRING);
	}
	else if (strcmp(argv[1], "pendulum") == 0){
		if (argc != 3){
			cout << "Enter the number of masses (>2)\n";
			return -1;
		}
		GLuint numMasses = (GLuint)atoi(argv[2]);
		if (numMasses <= 2){
			cout << "Number of masses must be greater than 2\n";
			return -1;
		}
		massSpring = new MassSpring((GLuint)PENDULUM, numMasses);
	}
	else if (strcmp(argv[1], "cube") == 0){
		massSpring = new MassSpring("cube.obj", CUBE);
		isObj = true;
	}
	else if (strcmp(argv[1], "cloth") == 0){
		massSpring = new MassSpring("cloth.obj", CLOTH);
		isObj = true;
	}
	else{
		cout << "Invalid arguments" << endl;
		return -1;
	}

	if (!glfwInit()){
		cout << "Couldn't initialize GLFW. \n";
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Mass-Spring Demo", NULL, NULL);
	if (!window)
	{
		if (shaderProgram) {
			glDeleteProgram(shaderProgram);
			glDeleteVertexArrays(1, &VAO);
		}
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		printf("Couldn't initialize GLEW.\n");
		return 1;
	}

        plane = new GOBJ("floor.obj");
	
	setupRenderingContext();
	if (shaderProgram) {
		while (!glfwWindowShouldClose(window))
		{
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);

			render();

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	delete massSpring;
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
