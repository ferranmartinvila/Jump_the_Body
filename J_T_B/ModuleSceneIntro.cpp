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
		if(k != 4)AddCentralColumns(&cube, 2.0f, 4.0f, 2.0f);
		AddSceneObject(&cube, STATIC_CUBE);
		cube = cube_2;
	}

	for (uint k = 0; k < 10; k++) {
		cube.AddAdjacentBody(&cube_2, -alpha * 0.3f, Z);
		if(k != 0)AddCentralColumns(&cube, 2.0f, 4.0f, 2.0f);
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
	Cube low_reception(45.0f, 0.2f, 60.f);
	low_reception.SetMultiRotation(cube.rotations.x, cube.rotations.y, cube.rotations.z);
	cube.AddAdjacentBody(&low_reception, 0, Y, 0, -15, -30);
	AddExternalColumns(&low_reception, 5.0f, 5.0f, 5.0f);
	AddSceneObject(&low_reception, STATIC_CUBE);
	// =========================================================
	
	
	// Up Half =================================================
	alpha = -45.0f;

	cube.ReSize(15.0f, 0.2f, 60.0f);
	low_reception.AddAdjacentBody(&cube, 0, Y, 0, 0, 0);
	AddSceneObject(&cube, STATIC_CUBE);
	cube_2 = cube;
	
	for (int k = 0; k < 20; k++) {
		if (k < 15)cube_2.ReSize(cube_2.size.x, cube_2.size.y, cube_2.size.z + k*0.5f);
		if (k < 15)cube.AddAdjacentBody(&cube_2, alpha * 0.1, Z,0.0f,0.0f,-k);
		else cube.AddAdjacentBody(&cube_2, alpha * 0.1, Z, 0.0f, 0.0f, 0.0f);
		if(k % 2 == 0 && k < 15)AddCentralColumns(&cube_2, 5.0f, 4.0f, 5.0f);
		AddSceneObject(&cube_2, STATIC_CUBE);
		cube = cube_2;
	}
	// =========================================================


	// Down Half ================================================
	cube.ReSize(35.0f, 0.2f, 60.0f);
	cube.rotations = low_reception.rotations;
	low_reception.AddAdjacentBody(&cube, 0, Y, -20 * cos(alpha * 0.1 * DEGTORAD), 0, -cube.size.z - cube_2.size.z - low_reception.size.z);
	AddSceneObject(&cube, STATIC_CUBE);
	cube_2 = cube;

	Cube stairs_base = cube;

	for (int k = 0; k < 20; k++) {
		cube_2.ReSize(15.0f, 0.2f, 60.0f + k * 3.0f);
		cube.AddAdjacentBody(&cube_2, alpha * 0.1, Z, 0.0f, 0.0f,0);
		if (k % 2 == 0 && k < 15)AddCentralColumns(&cube_2, 5.0f, 4.0f, 5.0f);
		AddSceneObject(&cube_2, STATIC_CUBE);
		cube = cube_2;
	}
	// =========================================================


	// Super Base ==============================================
	stairs_base.rotations.y += 180;
	cube.rotations = stairs_base.rotations;
	cube.ReSize(stairs_base.size.x + 65.0f, stairs_base.size.y, stairs_base.size.z);
	stairs_base.AddAdjacentBody(&cube, 0, Y, 0, 0, 0);
	AddCentralColumns(&cube, 5.0f, 33.0f, 5.0f);
	AddCentralColumns(&stairs_base, 5.0f, 25.0f, 5.0f);
	AddSceneObject(&cube, STATIC_CUBE);
	cube_2 = cube;
	// =========================================================


	// Super Stairs ============================================
	cube_2.ReSize(20.0f, cube_2.size.y, cube_2.size.z);

	for (int k = 0; k < 4; k++) {

		cube.AddAdjacentBody(&cube_2, 90, Z, 0, 0, 0);
		AddSceneObject(&cube_2, STATIC_CUBE);
		cube_2.ReSize(cube_2.size.x, cube_2.size.y, cube_2.size.z - 7.5f);
		cube.ReSize(cube.size.x, cube.size.y, cube.size.z - 7.5f);
		cube_2.AddAdjacentBody(&cube, -90, Z, 0, 0, 0);
		AddCentralColumns(&cube_2, 5.0f, 35.0f, 5.0f);
		AddSceneObject(&cube, STATIC_CUBE);
		
	}
	// =========================================================


	// Post Stairs Curve Base ==================================
	Cube stairs_curve_start = cube;
	stairs_curve_start.ReSize(35.0f, cube.size.y, cube.size.z);
	cube.AddAdjacentBody(&stairs_curve_start, 0, Z, -50, 0, 0);
	AddExternalColumns(&stairs_curve_start, 5.0f, 0.0f, 5.0f);
	AddSceneObject(&stairs_curve_start, STATIC_CUBE);
	// =========================================================


	// Post Stairs Curve =======================================
	alpha = -180.0f;
	cube.ReSize(20.0f, cube.size.y, cube.size.z);
	cube_2 = cube;

	stairs_curve_start.AddAdjacentBody(&cube, 0, Y, 0, 0, 0);
	AddSceneObject(&cube, STATIC_CUBE);

	for (int k = 0; k < 50; k++) {
		cube.AddAdjacentBody(&cube_2, alpha * 0.02f, Y);
		AddCentralColumns(&cube, 5.0f, 4.0f, 5.0f);
		cube = cube_2;
		AddSceneObject(&cube, STATIC_CUBE);
	}
	// =========================================================


	// Two Ways Ramp ===========================================
	alpha = -20;

	Cube two_ways_ramp = cube;
	cube.AddAdjacentBody(&two_ways_ramp, alpha * 0.1f, Z, 0, 0, 0);
	AddSceneObject(&two_ways_ramp, STATIC_CUBE);

	for (int k = 0; k < 9; k++) {

		cube_2.ReSize(cube_2.size.x, cube_2.size.y, cube_2.size.z + k * 2);
		cube.AddAdjacentBody(&cube_2, alpha * 0.1, Z, 0.0f, 0.0f, -k);
		AddCentralColumns(&cube, 5.0f, 4.0f, 5.0f);
		cube = cube_2;
		AddSceneObject(&cube_2, STATIC_CUBE);

	}
	AddCentralColumns(&cube, 5.0f, 4.0f, 5.0f);
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



