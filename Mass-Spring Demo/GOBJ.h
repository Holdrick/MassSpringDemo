#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "math.h"

using namespace std;
using namespace glm;


class GOBJ{
public:
	GOBJ(string filename);

	void readObj(string filename);
	void setupObj(string filename);
	void rearrangeData();
	void buffer();
	GLuint bufferSize();
	GLfloat* getData(GLuint type);
	GLuint getSize(GLuint type);
	vec3 findMax();
	vec3 findMin();
	void findCenter();
	void draw(mat4 model, vec3 position, GLuint mv);
	void draw(vec3 ambient, GLuint amb);


	vector<GLuint> indices, normIndices, texIndices;
	vector<GLfloat> vertices, normals, texVertices;
	vec3 center;

private:
	GLuint VAO, VBO;
};
