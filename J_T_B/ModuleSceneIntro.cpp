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

	App->camera->Move(vec3(0.0f, 0.0f, 0.0f));
	App->camera->LookAt(vec3(0, 1, 0));
	
	// Initial Ramp ============================================
	float alpha = 0;
	Cube cube;
	Cube cube_2;

	//Start Floor
	Cube start_floor(30.0f, 0.2f, 30.0f);
	start_floor.SetPos(0, start_floor.size.y + 120, 0);
	AddCentralColumns(&start_floor, start_floor.size.x, 12.0f, 10.0f);
	AddSceneObject(&start_floor, STATIC_CUBE);

	//Start Sec Wall
	Cube ramp_wall(0.3f, 8.0f, 30.0f);
	ramp_wall.SetPos(0 - start_floor.size.x * 0.5f + ramp_wall.size.x * 0.5f, start_floor.transform.translation().y + ramp_wall.size.y * 0.5f, 0);
	AddSceneObject(&ramp_wall, STATIC_CUBE);

	//Start Ramp
	alpha = 40;
	cube.ReSize(10.0f, .02f, 30.0f);
	cube_2.ReSize(10.0f, .02f, 30.0f);
	start_floor.AddAdjacentBody(&cube, alpha, Z);
	AddCentralColumns(&cube, 2.0f, 4.0f, 2.0f);
	AddSceneObject(&cube, STATIC_CUBE);
	
	for (uint k = 0; k < 6; k++) {
		cube.AddAdjacentBody(&cube_2, alpha * 0.15f, Z);
		AddCentralColumns(&cube, 2.0f, 4.0f, 2.0f);
		AddSceneObject(&cube, STATIC_CUBE);
		cube = cube_2;
	}

	for (uint k = 0; k < 10; k++) {
		cube.AddAdjacentBody(&cube_2, -alpha * 0.3f, Z);
		AddCentralColumns(&cube, 2.0f, 4.0f, 2.0f);
		AddSceneObject(&cube, STATIC_CUBE);
		cube = cube_2;
	}
	// =========================================================
	
	
	// High Reception ==========================================
	Cube high_reception(80.0f, 0.2f, 30.0f);
	high_reception.SetPosFrom((Primitive*)&cube, 0 + cube.size.x * 0.5f + high_reception.size.x * 0.5f + 50.0f, 0, 0);
	AddExternalColumns(&high_reception, 5.0f, 5.0f, 5.0f);
	AddSceneObject(&high_reception, STATIC_CUBE);
	// =========================================================


	// Sky Curve ===============================================
	cube.SetRotation(0, { 0,0,1 });
	cube.rotations = { 0,0,0 };
	cube.ReSize(40.0f, 0.2f, 30.f);
	cube_2 = cube;

	alpha = 90.0f;

	cube.SetPosFrom((Primitive*)&high_reception, 0 + high_reception.size.x * 0.5f + cube.size.x * 0.5f + 20.0f, 10.0f, 0);
	AddCentralColumns(&cube, 5.0f, 5.0f, 5.0f);
	AddSceneObject(&cube, STATIC_CUBE);

	for (uint k = 0; k < 10; k++) {
		cube.AddAdjacentBody(&cube_2, alpha * 0.12f, Y);
		AddCentralColumns(&cube, 5.0f, 4.0f, 5.0f);
		AddSceneObject(&cube, STATIC_CUBE);
		cube = cube_2;
	}
	// =========================================================


	// Low Reception ===========================================
	Cube low_raception(30.0f, 0.2f, 30.f);
	cube.AddAdjacentBody(&low_raception, 0, Y, 50, -15);
	AddCentralColumns(&low_raception, 5.0f, 5.0f, 5.0f);
	AddSceneObject(&low_raception, STATIC_CUBE);
	
	Cube half(30.0f, 0.2f, 30.f);
	low_raception.AddAdjacentBody(&half, 25, Z);
	AddCentralColumns(&half, 5.0f, 5.0f, 5.0f);
	AddSceneObject(&half, STATIC_CUBE);

	Cube half_2(30.0f, 0.2f, 30.f);
	half.AddAdjacentBody(&half_2, -25.0f, Z);
	AddCentralColumns(&half_2, 5.0f, 5.0f, 5.0f);
	AddSceneObject(&half_2, STATIC_CUBE);


	Cube half_3(30.0f, 0.2f, 30.f);
	half_2.AddAdjacentBody(&half_3, -55.0f, Z);
	AddCentralColumns(&half_3, 5.0f, 5.0f, 5.0f);
	AddSceneObject(&half_3, STATIC_CUBE);

	Cube half_4(30.0f, 0.2f, 30.f);
	half_3.AddAdjacentBody(&half_4, 55.0f, Z);
	AddCentralColumns(&half_4, 5.0f, 5.0f, 5.0f);
	AddSceneObject(&half_4, STATIC_CUBE);


	// =========================================================
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

void ModuleSceneIntro::AddCentralColumns(Primitive * target, float x, float y, float z)
{
	//Get the target position
	vec3 target_pos = ((Cube*)target)->transform.translation();

	//Calculate the vector from the position to the center of the sides
	vec3 Apoint(0.0f, 0.0f, -(((Cube*)target)->size.z * 0.5f) - z * 0.5f);
	Apoint = (rotate(Apoint, target->rotations.y, {0,-1.0f,0}));
	
	//Create the columns whit the data calculated
	Cube col_r(x, target_pos.y + y, z, target_pos.x + Apoint.x, ((target_pos.y + y) * 0.5f) + Apoint.y, target_pos.z + Apoint.z);
	Cube col_l(x, target_pos.y + y, z, target_pos.x - Apoint.x, ((target_pos.y + y) * 0.5f) + Apoint.y, target_pos.z - Apoint.z);
	//Rotate the new columns
	col_r.SetRotation(target->rotations.y, { 0,-1.0f,0 });
	col_l.SetRotation(target->rotations.y, { 0,-1.0f,0 });
	//Add it to the scene
	AddSceneObject(&col_r,STATIC_CUBE);
	AddSceneObject(&col_l, STATIC_CUBE);
}

void ModuleSceneIntro::AddExternalColumns(Primitive * target, float x, float y, float z)
{
	//Get the target position
	vec3 target_pos = ((Cube*)target)->transform.translation();

	vec3 Apoint_up((((Cube*)target)->size.x * 0.5f) + x*0.5f, 0.0f, -(((Cube*)target)->size.z * 0.5f) - z * 0.5f);
	vec3 Apoint_down((((Cube*)target)->size.x * 0.5f) + x*0.5f, 0.0f, +(((Cube*)target)->size.z * 0.5f) + z * 0.5f);

	Apoint_up = (rotate(Apoint_up, target->rotations.y, { 0,-1.0f,0 }));
	Apoint_down = (rotate(Apoint_down, target->rotations.y, { 0,-1.0f,0 }));

	Cube col_r_up(x, target_pos.y + y, z, target_pos.x + Apoint_up.x, ((target_pos.y + y) * 0.5f) + Apoint_up.y, target_pos.z + Apoint_up.z);
	Cube col_r_down(x, target_pos.y + y, z, target_pos.x + Apoint_down.x, ((target_pos.y + y) * 0.5f) + Apoint_down.y, target_pos.z + Apoint_down.z);
	Cube col_l_up(x, target_pos.y + y, z, target_pos.x - Apoint_up.x, ((target_pos.y + y) * 0.5f) + Apoint_up.y, target_pos.z - Apoint_up.z);
	Cube col_l_down(x, target_pos.y + y, z, target_pos.x - Apoint_down.x, ((target_pos.y + y) * 0.5f) + Apoint_down.y, target_pos.z - Apoint_down.z);

	//Rotate the new columns
	col_r_up.SetRotation(target->rotations.y, { 0,-1.0f,0 });
	col_r_down.SetRotation(target->rotations.y, { 0,-1.0f,0 });
	col_l_up.SetRotation(target->rotations.y, { 0,-1.0f,0 });
	col_l_down.SetRotation(target->rotations.y, { 0,-1.0f,0 });
	//Add it to the scene
	AddSceneObject(&col_r_up, STATIC_CUBE);
	AddSceneObject(&col_r_down, STATIC_CUBE);
	AddSceneObject(&col_l_up, STATIC_CUBE);
	AddSceneObject(&col_l_down, STATIC_CUBE);
}

PhysBody3D* ModuleSceneIntro::AddSceneObject(Primitive* object, OBJECT_TYPE object_type, float mass)
{
	Primitive* new_object = nullptr;
	
	switch (object_type)
	{
	
	case OBJECT_TYPE::DINAMIC_CUBE: new_object = new Cube(*(Cube*)object); break;
	case OBJECT_TYPE::STATIC_CUBE: new_object = new Cube(*(Cube*)object); break;
	case OBJECT_TYPE::DINAMIC_CYLINDER: new_object = new Cylinder(*(Cylinder*)object); break;
	case OBJECT_TYPE::DINAMIC_PLANE: new_object = new Plane(*(Plane*)object); break;
	case OBJECT_TYPE::DINAMIC_SPHERE: new_object = new Sphere(*(Sphere*)object); break;

	}

	graph_bodies.add(new_object);
	PhysBody3D* phys = App->physics->AddBody(object, object_type,mass);
	phys->collision_listeners.add(this);
	phys_bodies.add(phys);

	return phys;
}



