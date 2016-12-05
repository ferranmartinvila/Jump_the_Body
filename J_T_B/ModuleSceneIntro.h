#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"

struct PhysBody3D;

enum OBJECT_TYPE{

	DINAMIC_CUBE,
	STATIC_CUBE,
	DINAMIC_CYLINDER,
	STAIC_CYLINDER,
	DINAMIC_PLANE,
	STATIC_PLANE,
	DINAMIC_SPHERE,
	STATIC_SPHERE

};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

	//Physic bodies list
	p2List<PhysBody3D*>			phys_bodies;
	//Graphic bodies list
	p2List<Primitive*>			graph_bodies;

	//Map bodies array
	p2DynArray<PhysBody3D*>		map_bodies;
	//Map grahics array
	p2DynArray<Primitive*>		map_graphs;

	//Sphere Data
	PhysBody3D* b;
	Sphere* sphere;

public:

	void			OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void			AddCentralColumns(Primitive* target, float x, float y, float z);
	void			AddExternalColumns(Primitive* target, float x, float y, float z);
	
	PhysBody3D*		AddSceneObject(Primitive* object, OBJECT_TYPE object_type, float mass = 1.0f);
	PhysBody3D*		AddMapObject(Primitive* object, OBJECT_TYPE object_type, float mass = 1.0f);
	void			AddAdjacentBody(Primitive* origin, Primitive* target, float angle, AXIS axis, float x = 0, float y = 0, float z = 0);
};
