#include "MassSpring.h"

//Ball on spring
MassSpring::MassSpring(GLfloat mass, GLuint id){
	particles.push_back(new Particle(vec3(0.0f, 2.0f, 0.0f)));
	particles.push_back(new Particle(vec3(0.0f, 1.5f, 0.0f)));

	particles[1]->mass = mass;

	Spring * spring = new Spring(0, 1);
	spring->k = 3.0f;

	Pair pair = make_pair(0, 1);
	SpringPair springPair = make_pair(pair, spring);
	springMap.insert(springPair);

	this->id = id;
	this->damp = 0.5f;
}

//Pendulum
MassSpring::MassSpring(GLuint id, GLuint numMasses){
	vec3 location = vec3(0.0f, 2.0f, 0.0f);
	GLfloat spaceBw = 1.0f;

	for (GLuint i = 0; i < numMasses; i++){
		particles.push_back(new Particle(location));
		location.x += spaceBw;
	}

	particles[numMasses - 1]->mass = 5.0f;

	for (GLuint i = 0; i < particles.size()-1; i++){
		Spring * spring = new Spring(i, i+1);

		Pair pair = make_pair(i, i+1);
		SpringPair springPair = make_pair(pair, spring);
		springMap.insert(springPair);
	}

	this->id = id;
	this->damp = DAMP;
}

//Cube of Jelly or Cloth
MassSpring::MassSpring(string filename, GLuint id){
	gobj = new GOBJ(filename);
	vector<vec3> objVerts = floatToVec3(gobj->vertices);
	vector<GLuint> objIndices = gobj->indices;

	for (GLuint i = 0; i < objVerts.size(); i++){
		particles.push_back(new Particle(objVerts[i]));
	}

	for (GLuint i = 0; i < objIndices.size(); i+=3){
		if (objIndices[i] != objIndices[i + 1]){
			Pair pair1 = make_pair(objIndices[i], objIndices[i + 1]);
			Pair pair2 = make_pair(objIndices[i + 1], objIndices[i + 2]);
			Pair pair3 = make_pair(objIndices[i], objIndices[i + 2]);

			Spring * spring1 = new Spring("obj", objIndices[i], objIndices[i + 1], length(particles[objIndices[i]]->position - particles[objIndices[i+1]]->position));
			Spring * spring2 = new Spring("obj", objIndices[i + 1], objIndices[i + 2], length(particles[objIndices[i+1]]->position - particles[objIndices[i+2]]->position));
			Spring * spring3 = new Spring("obj", objIndices[i], objIndices[i + 2], length(particles[objIndices[i]]->position - particles[objIndices[i+2]]->position));

			springMap.insert(make_pair(pair1, spring1));
			springMap.insert(make_pair(pair2, spring2));
			springMap.insert(make_pair(pair3, spring3));
		}
	}

	if (filename == "cloth.obj"){
		circlePos = vec3(0.0f, -6.0f, 0.0f);
		radius = 4.0f;
		ambient = vec3(1.0f, 1.0f, 1.0f);
	}
	else{
		circlePos = vec3(0.0f, -300.0f, 0.0f);
		radius = 300.0f;
		ambient = vec3(0.0f, 0.0f, 1.0f);
	}

	this->id = id;
	this->damp = 4.0f;
}

MassSpring::~MassSpring(){
}

GLuint MassSpring::bufferSize()
{
	return sizeof(GLfloat)* vertices.size();
}

GLfloat* MassSpring::getData(GLuint type)
{
	return vertices.data();
}

GLuint MassSpring::getSize(GLuint type)
{
	return sizeof(GLfloat)* vertices.size();
}
vector<vec3> MassSpring::floatToVec3(vector<GLfloat> verts){
	vector<vec3> vs;

	for (GLuint i = 0; i < verts.size(); i += 4){
		vs.push_back(vec3(verts[i], verts[i + 1], verts[i + 2]));
	}

	return vs;
}
vector<GLfloat> MassSpring::makeVertices(vector<Particle*> parts){
	vector<GLfloat> verts;

	for (GLuint i = 0; i < parts.size(); i++){
		vec3 * position = &parts[i]->position;

		verts.push_back(position->x);
		verts.push_back(position->y);
		verts.push_back(position->z);
		verts.push_back(1.0f);
	}

	return verts;
}
int test = 0;
void MassSpring::update(){
	for (SpringIt iterator = springMap.begin(); iterator != springMap.end(); iterator++){
		Spring * s = iterator->second;

		Particle &p1 = *particles[s->i];
		Particle &p2 = *particles[s->j];
		
		GLfloat len = length(p1.position - p2.position);
		vec3 normVec = normalize(p1.position - p2.position);
		vec3 temp = (p1.velocity * normVec) - (p2.velocity * normVec);
		temp *= normVec * damp;

		vec3 force = -s->k*(len - s->l)*normVec - temp;

		p1.force += force;
		p2.force -= force;
	}

	for (GLuint i = id; i < particles.size(); i++){
		vec3 v = particles[i]->velocity;
		vec3 a = particles[i]->position;
		vec3 b = a + particles[i]->velocity*DELTAT;
		vec3 fColl = vec3(0.0f);
		vec3 n = vec3(0.0f, 1.0f, 0.0f);

		if (id == CUBE || id == CLOTH){
			if (length(b - circlePos) < radius){
				GLfloat l = length(b - circlePos) - radius;
				n = (b - circlePos) / length(b - circlePos);
				fColl = (-KCOLL*l*n) - (KCOLLDAMP*v);
			}
		}

		particles[i]->force += (GRAVITY*particles[i]->mass) + fColl;

		particles[i]->velocity += (particles[i]->force / particles[i]->mass)*DELTAT;
		particles[i]->position += particles[i]->velocity*DELTAT;

		particles[i]->force = vec3(0.0f);
	}
}

//Buffer and draw spring/pendulum
void MassSpring::draw(){
	vertices = makeVertices(particles);

	GLuint offset = 0;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferSize(), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER,
		offset,
		getSize(VERTEX_DATA),
		getData(VERTEX_DATA));

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(VERTEX_DATA);
	glVertexAttribPointer(VERTEX_DATA, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)offset);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, vertices.size()/4);
}

//Draw spring or pendulum
void MassSpring::renderMS(){
	update();
	draw();
}

//Draw cube or cloth
void MassSpring::renderObj(GLuint amb){
	update();
	gobj->vertices = makeVertices(particles);
	gobj->rearrangeData();
	gobj->buffer();
	gobj->draw(ambient, amb);
}
