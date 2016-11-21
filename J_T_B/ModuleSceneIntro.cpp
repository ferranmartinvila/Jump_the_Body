#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	//Sphere Creation
	sphere = new Sphere(0.5f);
	sphere->SetPos(0, sphere->radius + 1.0f, 8.0f);
	b = AddSceneObject(sphere, SPHERE);
	sphere = (Sphere*)graph_bodies[0];

	//Cube Creation
	/*Cube cube(3.0f,3.0f, 3.0f);
	cube.SetPos(4.0f, cube.size.y + 1.0f, 4.0f);
	AddSceneObject(&cube, CUBE);*/
	
	//Ground Creation
	Cube roof(100.0f, 1.0f, 100.0f);
	roof.SetPos(0.0f, 25.0f, 0.0f);
	AddSceneObject(&roof,STATIC_CUBE);
	

	// =============================================
	Cube wall_1(1.0f, 6.0f, 12.0f);
	wall_1.SetPos(5.0f, 0.2f, 5.0f);
	AddSceneObject(&wall_1, CUBE, 100.0f);

	wall_1.SetPos(10.5f,  0.2f,11.0f);
	wall_1.SetRotation(90.0f,vec3(0, 1.0f, 0));
	AddSceneObject(&wall_1, CUBE, 100.0f);

	wall_1.SetPos(-0.5f, 0.2f, -1.5f);
	wall_1.SetRotation(-90.0f, vec3(0, 1.0f, 0));
	AddSceneObject(&wall_1, CUBE, 100.0f);
	
	// =============================================
	Cylinder jumper(1.0f, 0.2f);
	jumper.SetPos(10.0f, 0.2f, 10.0f);
	jumper.SetRotation(90, vec3(0, 0, 1.0f));
	AddSceneObject(&jumper, CYLINDER, 100.0f);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	
	//Phys bodies are deleted in ModulePhysics3D
	phys_bodies.clear();
	
	//Delete all the graphic bodies
	uint k = graph_bodies.count();
	for (int h = k - 1; h >= 0; h--) {
		delete graph_bodies[h];
		graph_bodies[h] = NULL;
	}
	graph_bodies.clear();

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	//Render the plane
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	//Check for the number of items (phys & graph)
	uint scene_items_num = phys_bodies.count();
	assert(scene_items_num == graph_bodies.count(), "Scene items error.");
	
	//Update scene items
	for (uint k = 0; k < scene_items_num; k++) {
		phys_bodies[k]->GetTransform(&graph_bodies[k]->transform);
		graph_bodies[k]->Render();
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D * body1, PhysBody3D * body2)
{

	if (body1 == b || body2 == b)sphere->color = Red;

}

PhysBody3D* ModuleSceneIntro::AddSceneObject(Primitive* object, OBJECT_TYPE object_type, float mass)
{
	Primitive* new_object = nullptr;
	
	switch (object_type)
	{
	
	case OBJECT_TYPE::CUBE: new_object = new Cube(*(Cube*)object); break;
	case OBJECT_TYPE::STATIC_CUBE: new_object = new Cube(*(Cube*)object); break;
	case OBJECT_TYPE::CYLINDER: new_object = new Cylinder(*(Cylinder*)object); break;
	case OBJECT_TYPE::PLANE: new_object = new Plane(*(Plane*)object); break;
	case OBJECT_TYPE::SPHERE: new_object = new Sphere(*(Sphere*)object); break;

	}

	graph_bodies.add(new_object);
	PhysBody3D* phys = App->physics->AddBody(object, object_type,mass);
	phys->collision_listeners.add(this);
	phys_bodies.add(phys);

	return phys;
}



