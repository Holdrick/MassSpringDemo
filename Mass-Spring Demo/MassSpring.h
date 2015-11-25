#pragma once

#include <map>
#include <iterator>
#include "Spring.h"
#include "GOBJ.h"

typedef pair<int, int> Pair;
typedef pair<Pair, Spring*> SpringPair;
typedef map<Pair, Spring*> SpringMap;
typedef map<Pair, Spring*>::iterator SpringIt;

#define VERTEX_DATA 0
#define NORMAL_DATA 1

#define KCOLL 10.0f
#define KCOLLDAMP 2.0f

#define SPRING 1
#define PENDULUM 1
#define CUBE 0
#define CLOTH 0

const vec3 WHITE = vec3(1.0f, 1.0f, 1.0f);
const vec3 BLUE = vec3(0.0f, 0.0f, 1.0f);
const vec3 RED = vec3(1.0f, 0.0f, 0.0f);

struct CubeCorners{
	GLuint a, b, c, d;
	GLuint e, f, g, h;
};

struct ClothCorners{
	GLuint a, b, c, d;
};

class MassSpring
{
public:
	MassSpring(GLfloat mass, GLuint id);
	MassSpring(GLuint id, GLuint numMasses);
	MassSpring(string filename, GLuint id);
	~MassSpring();

	vector<GLfloat> makeVertices(vector<Particle*> parts);
	vector<vec3> floatToVec3(vector<GLfloat> verts);

	void update();
	void draw();
	void renderMS();
	void renderObj(GLuint amb);

	GLuint bufferSize();
	GLfloat* getData(GLuint type);
	GLuint getSize(GLuint type);

	vec3 ambient;

private:
	vector<Particle*> particles;
	Particle ***cloth;
	vector<GLfloat> vertices;
	vector<vec3> vec3Vertices;
	vector<GLuint> indices;
	SpringMap springMap;
	GOBJ * gobj;
	vec3 circlePos;
	GLfloat radius, damp;

	GLuint VAO, VBO, id;
};