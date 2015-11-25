#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

const GLfloat DEFMASS = 2.0f;

class Particle
{
public:

	inline Particle(){
		force = vec3(0.0f);
		velocity = vec3(0.0f);
		mass = DEFMASS;
	}
	inline Particle(vec3 position){
		force = vec3(0.0f);
		velocity = vec3(0.0f);
		mass = DEFMASS;
		this->position = position;
	}

	vec3 position;
	vec3 velocity;
	vec3 force;
	GLfloat mass;
};
