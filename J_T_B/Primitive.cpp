#include"Application.h"
#include "ModuleSceneIntro.h"
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"
#include "glut/glut.h"


#pragma comment (lib, "glut/glut32.lib")

// ------------------------------------------------------------
Primitive::Primitive() : transform(IdentityMatrix), color(White), wire(false), axis(false), rotations({0,0,0}),type(PrimitiveTypes::Primitive_Point)
{}

Primitive::Primitive(float posX, float posY, float posZ) : transform(IdentityMatrix), color(White), wire(false), axis(false), rotations({ 0,0,0 }), type(PrimitiveTypes::Primitive_Point)
{
	transform.translate(posX, posY, posZ);
}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void Primitive::Render() const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	if(axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	if(wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	InnerRender();

	glPopMatrix();
}

// ------------------------------------------------------------
void Primitive::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}

void Primitive::SetPosFrom(Primitive * origin, float x, float y, float z)
{
	vec3 org = origin->transform.translation();
	transform.translate(org.x + x, org.y + y, org.z + z);
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const vec3 &u)
{
	transform.rotate(angle, u);

}

void Primitive::SetMultiRotation(float x, float y, float z)
{

	mat4x4 rot_x = transform.rotate(x, { -1,0,0 });
	mat4x4 rot_y = transform.rotate(y, { 0,-1,0 });
	mat4x4 rot_z = transform.rotate(z, { 0,0,-1 });
	transform = rot_x* rot_y *rot_z;
}

void Primitive::AddAdjacentBody(Primitive * target, float angle, AXIS axis, float x, float y, float z)
{
	//Calculate the point of the parent object
	vec3 Apoint((((Cube*)this)->size.x * 0.5f), 0.0f, - (((Cube*)this)->size.z * 0.5f));

	Apoint = (rotate(Apoint, this->rotations.y, { 0,-1.0f,0 }));
	Apoint = (rotate(Apoint, this->rotations.z, { 0,0,-1.0f }));		
	

	//Calculate the point in the child object(whitout rotate)
	vec3 Bpoint(Apoint.x + (((Cube*)target)->size.x * 0.5f) + x, 0.0f + y, Apoint.z + (((Cube*)target)->size.z * 0.5f) + z);
	
	//Vector form A to B
	vec3 vector(Bpoint.x - Apoint.x, Bpoint.y - Apoint.y, Bpoint.z - Apoint.z);

	//Rotate the entered angle
	switch (axis) {

	case AXIS::Y:
		vector = rotate(vector, this->rotations.z, { 0,0,-1.0f });
		vector = rotate(vector, this->rotations.y + angle, { 0,-1.0f,0 });
		//Update the body angle
		target->rotations.y = angle + this->rotations.y;
		target->SetRotation(angle + this->rotations.y, { 0,-1.0f,0 });
		break;

	case AXIS::Z:
		vector = rotate(vector, this->rotations.z + angle, { 0,0,-1.0f });
		vector = rotate(vector, this->rotations.y, { 0,-1.0f,0 });
		//Update the body angle
		target->rotations.z = angle + this->rotations.z;
		target->rotations.y = this->rotations.y;

		target->SetMultiRotation(0, this->rotations.y, angle + this->rotations.z);
		//target->SetRotation(this->rotations.y, { 0,-1.0f,0 });
		
		//target->SetRotation(angle + this->rotations.z, { 0,0,-1.0f });
		break;

	}
	//Set the data calculated
	target->SetPosFrom(this, Apoint.x + vector.x, Apoint.y + vector.y, Apoint.z + vector.z);
	
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

// CUBE ============================================
Cube::Cube() : Primitive(), size(1.0f, 1.0f, 1.0f)
{
	type = PrimitiveTypes::Primitive_Cube;
}

Cube::Cube(float sizeX, float sizeY, float sizeZ) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;
}

Cube::Cube(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ) : Primitive(posX,posY,posZ), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;
}

void Cube::InnerRender() const
{	
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f( sx, -sy, sz);
	glVertex3f( sx,  sy, sz);
	glVertex3f(-sx,  sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f( sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx,  sy, -sz);
	glVertex3f( sx,  sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy,  sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx,  sy, -sz);
	glVertex3f(sx,  sy,  sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy,  sz);
	glVertex3f(-sx,  sy,  sz);
	glVertex3f(-sx,  sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy,  sz);
	glVertex3f( sx, sy,  sz);
	glVertex3f( sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f( sx, -sy, -sz);
	glVertex3f( sx, -sy,  sz);
	glVertex3f(-sx, -sy,  sz);

	glEnd();
}

void Cube::ReSize(float x, float y, float z)
{
	size.x = x;
	size.y = y;
	size.z = z;
}

// SPHERE ============================================
Sphere::Sphere() : Primitive(), radius(1.0f)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

Sphere::Sphere(float radius) : Primitive(), radius(radius)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

Sphere::Sphere(float radius, float posX, float posY, float posZ) : Primitive(posX, posY, posZ), radius(radius)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

void Sphere::InnerRender() const
{
	glutSolidSphere(radius, 25, 25);
}


// CYLINDER ============================================
Cylinder::Cylinder() : Primitive(), radius(1.0f), height(1.0f)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

Cylinder::Cylinder(float radius, float height) : Primitive(), radius(radius), height(height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

Cylinder::Cylinder(float radius, float height, float posX, float posY, float posZ) : Primitive(posX, posY, posZ), radius(radius), height(height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void Cylinder::InnerRender() const
{
	int n = 30;

	// Cylinder Bottom
	glBegin(GL_POLYGON);
	
	for(int i = 360; i >= 0; i -= (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for(int i = 0; i <= 360; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_QUAD_STRIP);
	for(int i = 0; i < 480; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians

		glVertex3f(height*0.5f,  radius * cos(a), radius * sin(a) );
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a) );
	}
	glEnd();
}

// LINE ==================================================
Line::Line() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

Line::Line(float x, float y, float z) : Primitive(), origin(0, 0, 0), destination(x, y, z)
{
	type = PrimitiveTypes::Primitive_Line;
}

void Line::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
Plane::Plane() : Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

Plane::Plane(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void Plane::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 200.0f;

	for(float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}