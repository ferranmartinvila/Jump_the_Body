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
	sphere->SetPos(0, sphere->radius + 1.0f, 0);
	b = AddSceneObject(sphere, SPHERE);
	sphere = (Sphere*)graph_bodies[0];

	//Cube Creation
	Cube cube(3.0f,3.0f, 3.0f);
	cube.SetPos(4.0f, cube.size.y + 1.0f, 4.0f);
	AddSceneObject(&cube, CUBE);
	
	/*//Ground Creation
	AddSceneObject(&Cube(100.0f, 2.0f, 100.0f),CUBE);*/
	

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	
	//Phys bodies are deleted in ModulePhysics3D
	phys_bodies.Clear();
	
	//Delete all the graphic bodies
	uint k = graph_bodies.Count();
	for (int h = k - 1; h >= 0; h--) {
		delete graph_bodies[h];
		graph_bodies[h] = NULL;
	}
	graph_bodies.Clear();

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
	uint scene_items_num = phys_bodies.Count();
	assert(scene_items_num == graph_bodies.Count(), "Scene items error.");
	
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

PhysBody3D* ModuleSceneIntro::AddSceneObject(Primitive* object, OBJECT_TYPE object_type)
{
	Primitive* new_object = nullptr;
	
	switch (object_type)
	{
	
	case OBJECT_TYPE::CUBE: new_object = new Cube(*(Cube*)object); break;
	case OBJECT_TYPE::CYLINDER: new_object = new Cylinder(*(Cylinder*)object); break;
	case OBJECT_TYPE::PLANE: new_object = new Plane(*(Plane*)object); break;
	case OBJECT_TYPE::SPHERE: new_object = new Sphere(*(Sphere*)object); break;

	}

	graph_bodies.PushBack(new_object);
	PhysBody3D* phys = App->physics->AddBody(object, object_type);
	phys->collision_listeners.add(this);
	phys_bodies.PushBack(phys);

	return phys;
}



