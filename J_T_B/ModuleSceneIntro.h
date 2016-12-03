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

	//Physic bodies array
	p2List<PhysBody3D*> phys_bodies;

	//Graphic bodies array
	p2List<Primitive*> graph_bodies;

	//Sphere Data
	PhysBody3D* b;
	Sphere* sphere;

public:

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void	AddCentralColumns(Primitive* target, float x, float y, float z);
	void	AddExternalColumns(Primitive* target, float x, float y, float z);
	PhysBody3D*  AddSceneObject(Primitive* object, OBJECT_TYPE object_type, float mass = 1.0f);

};
