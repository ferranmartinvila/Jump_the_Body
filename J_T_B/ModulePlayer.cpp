#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(6, 1, 8);
	car.chassis_offset.Set(0, 1.5f, 0);
	car.mass = 500.0f;
	car.suspensionStiffness = 5.88f;
	car.suspensionCompression = 1.6f;
	car.suspensionDamping = 6.88f;
	car.maxSuspensionTravelCm = 10000.0f;
	car.frictionSlip = 12.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;

	vec3 direction(0, -1, 0);
	vec3 axis(-1, 0, 0);

	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width * 2;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width * 2;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0, 150, -10);

	print_cabine.ReSize(6, 2, 1);
	print_cabine.SetPos(vehicle->get_rigid_body()->getWorldTransform().getOrigin().x(), vehicle->get_rigid_body()->getWorldTransform().getOrigin().y() + 2.5f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().z() + 3.5f);

	print_door_1.ReSize(1, 2, 3);
	print_door_1.SetPos(vehicle->get_rigid_body()->getWorldTransform().getOrigin().x() + 2.5f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().y() + 2.5f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().z() + 1.0f);

	print_door_2.ReSize(1, 2, 3);
	print_door_2.SetPos(vehicle->get_rigid_body()->getWorldTransform().getOrigin().x() - 2.5f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().y() + 2.5f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().z() + 1.0f);

	print_Back.ReSize(6, 4, 3);
	print_Back.SetPos(vehicle->get_rigid_body()->getWorldTransform().getOrigin().x(), vehicle->get_rigid_body()->getWorldTransform().getOrigin().y() + 1.0f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().z() - 2.5f);

	print_roof.ReSize(6, 0.25, 8);
	print_roof.SetPos(vehicle->get_rigid_body()->getWorldTransform().getOrigin().x(), vehicle->get_rigid_body()->getWorldTransform().getOrigin().y() + 4.6f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().z());


	//Setting up the car elements
	cabine = App->physics->AddBody(&print_cabine, OBJECT_TYPE::DINAMIC_CUBE);
	door_1 = App->physics->AddBody(&print_door_1, OBJECT_TYPE::DINAMIC_CUBE);
	door_2 = App->physics->AddBody(&print_door_2, OBJECT_TYPE::DINAMIC_CUBE);
	Back = App->physics->AddBody(&print_Back, OBJECT_TYPE::DINAMIC_CUBE);
	roof = App->physics->AddBody(&print_roof, OBJECT_TYPE::DINAMIC_CUBE);
	
	//Setting hinges
	cabine_to_vehicle =  App->physics->Add_Hinge_Constraint(*vehicle->get_rigid_body(), *cabine->get_rigid_body(), { 0,3.0f,3.5f }, { 0,0,0 },  btVector3(0,1,0), btVector3(0, 1, 0));
	cabine_to_vehicle->setLimit(0, ((2*3.1416)/360));

	door_1_constrain = App->physics->Add_Hinge_Constraint(*vehicle->get_rigid_body(), *door_1->get_rigid_body(), { 2.5f, 3.0f, 2.5f }, { 0,0,-1.5f }, btVector3(0, 1, 0), btVector3(0, 1, 0));
	door_1_constrain->setLimit(-3.1416, 0);

	door_2_constrain = App->physics->Add_Hinge_Constraint(*vehicle->get_rigid_body(), *door_2->get_rigid_body(), { -2.5f, 3.0f, 2.5f }, { 0,0,-1.5f }, btVector3(0, 1, 0), btVector3(0, 1, 0));
	door_2_constrain->setLimit(0, 3.1416);

	Back_constrain = App->physics->Add_Hinge_Constraint(*vehicle->get_rigid_body(), *Back->get_rigid_body(), { 0, 4.0f, -2.5f }, { 0,0,0 }, btVector3(0, 1, 0), btVector3(0, 1, 0));
	Back_constrain->setLimit(0, ((2 * 3.1416) / 360));

	roof_constrain = App->physics->Add_Hinge_Constraint(*Back->get_rigid_body(), *roof->get_rigid_body(), { 0, 2, 0 }, { 0, -0.125f,-2.5f }, btVector3(0, 1, 0), btVector3(0, 1, 0));
	roof_constrain->setLimit(0, ((2 * 3.1416) / 360));


	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		acceleration = MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		if(vehicle->GetKmh() < 0.4f)acceleration = -MAX_ACCELERATION;
		else brake = BRAKE_POWER;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (turn < TURN_DEGREES)
			turn += TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	}
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		bool kk = vehicle->vehicle->m_wheelInfo[0].m_raycastInfo.m_isInContact;
		if(kk)vehicle->Push(0, 2000, 0);
	}

	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	vehicle->Render();

	char title[80];
	sprintf_s(title, "%.1f Km/h", vehicle->GetKmh());
	App->window->SetTitle(title);


	cabine->GetTransform(print_cabine.transform.M);
	door_1->GetTransform(print_door_1.transform.M);
	door_2->GetTransform(print_door_2.transform.M);
	Back->GetTransform(print_Back.transform.M);
	roof->GetTransform(print_roof.transform.M);

	print_cabine.Render();
	print_door_1.Render();
	print_door_2.Render();
	print_Back.Render();
	print_roof.Render();

	return UPDATE_CONTINUE;
}

