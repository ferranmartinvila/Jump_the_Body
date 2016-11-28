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
	/*sphere = new Sphere(0.5f, 0, 0.5f + 1.0f, 8.0f);
	b = AddSceneObject(sphere, DINAMIC_SPHERE);
	sphere = (Sphere*)graph_bodies[0];*/
	
	//Cube Creation
	/*Cube cube(3.0f,3.0f, 3.0f);
	cube.SetPos(4.0f, cube.size.y + 1.0f, 4.0f);
	AddSceneObject(&cube, CUBE);*/
	
	//Roof Creation
	/*Cube roof(100.0f, 1.0f, 100.0f);
	roof.SetPos(0.0f, 25.0f, 0.0f);
	AddSceneObject(&roof,STATIC_CUBE);*/
	

	// =============================================
	/*Cube wall_1(1.0f, 6.0f, 12.0f);
	wall_1.SetPos(5.0f, wall_1.size.y/2, 5.0f);
	AddSceneObject(&wall_1, STATIC_CUBE, 100.0f);

	wall_1.SetPos(10.5f, wall_1.size.y / 2,11.0f);
	wall_1.SetRotation(90.0f,vec3(0, 1.0f, 0));
	AddSceneObject(&wall_1, STATIC_CUBE, 100.0f);

	wall_1.SetPos(-0.5f, wall_1.size.y / 2, -1.5f);
	wall_1.SetRotation(-90.0f, vec3(0, 1.0f, 0));
	AddSceneObject(&wall_1, STATIC_CUBE, 100.0f);*/
	
	// =============================================
	/*Cylinder jumper(1.0f, 0.2f);
	jumper.SetPos(10.0f, 0.2f, 10.0f);
	jumper.SetRotation(-90, vec3(0, 0, 1.0f));
	AddSceneObject(&jumper, DINAMIC_CYLINDER);*/

	// =============================================
	
	// Initial Ramp ============================================
	int alpha = 35;

	Cube start_floor(20.0f, 0.2f, 30.0f);
	start_floor.SetPos(0, start_floor.size.y + 80, 0);
	AddSceneObject(&start_floor, STATIC_CUBE);
	
	Cube left_tower(15.0f, 90.0f, 15.0f);
	left_tower.SetPos(-start_floor.size.x * 0.5f + left_tower.size.x *0.5f, left_tower.size.y*0.5, -start_floor.size.z * 0.5f - left_tower.size.z *0.5f);
	AddSceneObject(&left_tower, STATIC_CUBE);

	Cube right_tower(15.0f, 90.0f, 15.0f);
	right_tower.SetPos(-start_floor.size.x * 0.5f + right_tower.size.x *0.5f, right_tower.size.y*0.5, start_floor.size.z * 0.5f + right_tower.size.z *0.5f);
	AddSceneObject(&right_tower, STATIC_CUBE);

	Cube ramp_wall(0.3f, 8.0f, 30.0f);
	ramp_wall.SetPos(0 - start_floor.size.x * 0.5f + ramp_wall.size.x * 0.5f, start_floor.transform.translation().y + ramp_wall.size.y * 0.5f, 0);
	AddSceneObject(&ramp_wall, STATIC_CUBE);

	Cube initial_ramp(70.0f, 0.2f, 30.0f);
	initial_ramp.SetPosFrom((Primitive*)&start_floor, 0 + start_floor.size.x *0.5f + GET_X_LNG(70, alpha)), -GET_Y_LNG(70, alpha)), 0);
	initial_ramp.SetRotation(alpha, { 0,0,-1 });
	AddSceneObject(&initial_ramp, STATIC_CUBE);

	Cube ramp_floor(40.0f, 0.2f, 30.0f);
	ramp_floor.SetPosFrom((Primitive*)&initial_ramp, GET_X_LNG(70, alpha)) + ramp_floor.size.x * 0.5f, -GET_Y_LNG(70, alpha)),0);
	AddSceneObject(&ramp_floor, STATIC_CUBE);

	Cube little_left_tower(5.0f, 50.0f, 5.0f);
	little_left_tower.SetPosFrom((Primitive*)&ramp_floor, -ramp_floor.size.x * 0.5 + little_left_tower.size.x *0.5, -ramp_floor.transform.translation().y + little_left_tower.size.y *0.5, -ramp_floor.size.z * 0.5f - little_left_tower.size.z *0.5);
	AddSceneObject(&little_left_tower, STATIC_CUBE);

	Cube little_right_tower(5.0f, 50.0f, 5.0f);
	little_right_tower.SetPosFrom((Primitive*)&ramp_floor, -ramp_floor.size.x * 0.5 + little_right_tower.size.x *0.5, -ramp_floor.transform.translation().y + little_right_tower.size.y *0.5, ramp_floor.size.z * 0.5f + little_right_tower.size.z *0.5);
	AddSceneObject(&little_right_tower, STATIC_CUBE);

	Cube little_left_tower_2(5.0f, 50.0f, 5.0f);
	little_left_tower_2.SetPosFrom((Primitive*)&ramp_floor, ramp_floor.size.x * 0.5 - little_left_tower_2.size.x *0.5, -ramp_floor.transform.translation().y + little_left_tower_2.size.y *0.5, -ramp_floor.size.z * 0.5f - little_left_tower_2.size.z *0.5);
	AddSceneObject(&little_left_tower_2, STATIC_CUBE);

	Cube little_right_tower_2(5.0f, 50.0f, 5.0f);
	little_right_tower_2.SetPosFrom((Primitive*)&ramp_floor, ramp_floor.size.x * 0.5 - little_right_tower_2.size.x *0.5, -ramp_floor.transform.translation().y + little_right_tower_2.size.y *0.5, ramp_floor.size.z * 0.5f + little_right_tower_2.size.z *0.5);
	AddSceneObject(&little_right_tower_2, STATIC_CUBE);
	// =========================================================



	/*Cube floor_b(floor);
	floor_b.SetPosFrom((Primitive*)&floor, floor.size.x, 0, 0);
	AddSceneObject(&floor_b, STATIC_CUBE);

	/*float y = GET_Y_LNG(30, 25));
	float x = GET_X_LNG(30, 25));

	Cube floor_2(20.0f, 0.2f, 30.0f);
	floor_2.SetPos(0.0f,  GET_Y_LNG(30, 25)) + floor_2.size.y,floor.size.z*0.5 +  GET_X_LNG(30,25)));
	floor_2.SetRotation(-25, { 1,0,0 });
	AddSceneObject(&floor_2, STATIC_CUBE);

	/*floor.SetPos(8.0f, floor.size.y, 40.0f);
	floor.SetRotation(20, { 0,1,0 });
	AddSceneObject(&floor, STATIC_CUBE);*/



	// =============================================


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



