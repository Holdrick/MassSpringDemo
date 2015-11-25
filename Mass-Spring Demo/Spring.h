#pragma once

#include "Particle.h"

const GLfloat SPRINGCONSTANT = 10.0f;
const GLfloat SPRINGLENGTH = 0.5f;
const GLfloat DAMP = 2.0f;
const vec3 GRAVITY = vec3(0.0f, -1.0f, 0.0f);
const GLfloat DELTAT = 0.05f;

class Spring
{
public:

	inline Spring(GLfloat i, GLfloat j){
		k = SPRINGCONSTANT;
		l = SPRINGLENGTH;
		this->i = i;
		this->j = j;
	}

	inline Spring(string str, GLfloat i, GLfloat j, GLfloat l){
		k = 15.0f;
		this->l = l;
		this->i = i;
		this->j = j;
	}

	GLint i, j;
	GLfloat k, l;
};
