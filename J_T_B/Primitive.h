
#pragma once
#include "glmath.h"
#include "Color.h"

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder
};

class Primitive
{
public:

	Primitive();
	Primitive(float posX, float posY, float posZ);

	virtual void	Render() const;
	virtual void	InnerRender() const;

	//Map building functions ------------------------------------------------
	void			SetPos(float x, float y, float z);
	void			SetPosFrom(Primitive* origin, float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Move(float x, float y, float z);
	void			PutRotatedBody(Primitive* target, float angle, const vec3& axis);
	void			AddAdjacentBody(Primitive* target, float angle, const vec3& axis);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	mat4x4 transform;
	vec3 rotations;
	bool axis,wire;

protected:
	PrimitiveTypes type;
};

// ============================================
class Cube : public Primitive
{
public :
	Cube();
	Cube(float sizeX, float sizeY, float sizeZ);
	Cube(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);
	void InnerRender() const;
public:
	vec3 size;
};

// ============================================
class Sphere : public Primitive
{
public:
	Sphere();
	Sphere(float radius);
	Sphere(float radius, float posX, float posY, float posZ);
	void InnerRender() const;
public:
	float radius;
};

// ============================================
class Cylinder : public Primitive
{
public:
	Cylinder();
	Cylinder(float radius, float height);
	Cylinder(float radius, float height, float posX, float posY, float posZ);
	void InnerRender() const;
public:
	float radius;
	float height;
};

// ============================================
class Line : public Primitive
{
public:
	Line();
	Line(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
class Plane : public Primitive
{
public:
	Plane();
	Plane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec3 normal;
	float constant;
};