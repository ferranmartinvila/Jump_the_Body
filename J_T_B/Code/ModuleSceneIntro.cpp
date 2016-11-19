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
	b = App->physics->AddBody(*sphere);

	//Cube Creation
	cube = new Cube(3.0f,3.0f, 3.0f);
	cube->SetPos(4.0f, cube->size.y + 1.0f, 4.0f);
	cube_phys = App->physics->AddBody(*cube);

	//Add this module to the list of collision listeners
	b->collision_listeners.add(this);
	cube_phys->collision_listeners.add(this);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	//Render the plane
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	//Update sphere location and render it
	b->GetTransform(&sphere->transform);
	sphere->Render();

	//Update cube location and render it
	cube_phys->GetTransform(&cube->transform);
	cube->Render();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D * body1, PhysBody3D * body2)
{

	if (body1 == b || body2 == b)sphere->color = Red;
	else if (body1 == cube_phys || body2 == cube_phys)cube->color = Blue;

}



