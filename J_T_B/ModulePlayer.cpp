#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"
#include "ModuleAudio.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
	chronometer.Stop();
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	hydraulic_suspension_fx = App->audio->LoadFx("../Game/Audio/hydraulic_suspension_fx.wav");
	car_reset_fx = App->audio->LoadFx("../Game/Audio/reset.wav");

	engine_low_rpm_fx = App->audio->LoadFx("../Game/Audio/engine_low_rpm.wav");
	engine_mid_rpm_fx = App->audio->LoadFx("../Game/Audio/engine_mid_rpm.wav");
	engine_high_rpm_fx = App->audio->LoadFx("../Game/Audio/engine_high_rpm.wav");
	engine_max_rpm_fx = App->audio->LoadFx("../Game/Audio/engine_max_rpm.wav");
	turbo_fx = App->audio->LoadFx("../Game/Audio/turbo_fx.wav");


	VehicleInfo car;
	//Car engine Build
	engine_loop = 330;
	Mix_Volume(2, engine_low_vol);
	
	// Car properties ----------------------------------------
	car.chassis_size.Set(6, 5, 8);
	car.chassis_offset.Set(0, 3.5f, 0);
	car.mass = 1000.0f;
	car.suspensionStiffness = 10.0f;
	car.suspensionCompression = 0.0f;
	car.suspensionDamping = 8.88f;
	car.maxSuspensionTravelCm = 200.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 2.2f;

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
	vehicle->SetTransform(&App->scene_intro->checkpoints[0]);

	print_door_1.ReSize(1, 2, 3);
	print_door_1.SetPos(vehicle->get_rigid_body()->getWorldTransform().getOrigin().x() + 2.5f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().y() + 2.5f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().z() + 1.0f);

	print_door_2.ReSize(1, 2, 3);
	print_door_2.SetPos(vehicle->get_rigid_body()->getWorldTransform().getOrigin().x() - 2.5f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().y() + 2.5f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().z() + 1.0f);

	print_Back_Door.ReSize(4, 4, 1);
	print_Back_Door.SetPos(vehicle->get_rigid_body()->getWorldTransform().getOrigin().x(), vehicle->get_rigid_body()->getWorldTransform().getOrigin().y() + 4.0f, vehicle->get_rigid_body()->getWorldTransform().getOrigin().z() - 3.5f);


	//Setting up the car elements

	door_1 = App->physics->AddBody(&print_door_1, OBJECT_TYPE::DINAMIC_CUBE);
	door_2 = App->physics->AddBody(&print_door_2, OBJECT_TYPE::DINAMIC_CUBE);
	Back_Door = App->physics->AddBody(&print_Back_Door, OBJECT_TYPE::DINAMIC_CUBE);

	door_1->get_rigid_body()->setIgnoreCollisionCheck(vehicle->get_rigid_body(), true);
	door_2->get_rigid_body()->setIgnoreCollisionCheck(vehicle->get_rigid_body(), true);
	Back_Door->get_rigid_body()->setIgnoreCollisionCheck(vehicle->get_rigid_body(), true);
	//Setting hinges
	
	door_1_constrain = App->physics->Add_Hinge_Constraint(*vehicle->get_rigid_body(), *door_1->get_rigid_body(), { 2.5f, 3.0f, 2.5f }, { 0,0,-1.5f }, btVector3(0, 1, 0), btVector3(0, 1, 0));
	door_1_constrain->setLimit((3.1416f), (3 * (3.1416f / 2)));

	door_2_constrain = App->physics->Add_Hinge_Constraint(*vehicle->get_rigid_body(), *door_2->get_rigid_body(), { -2.5f, 3.0f, 2.5f }, { 0,0,-1.5f }, btVector3(0, 1, 0), btVector3(0, 1, 0));
	door_2_constrain->setLimit((3.1416f / 2), 3.1416f);

	Back_Door_constrain = App->physics->Add_Hinge_Constraint(*vehicle->get_rigid_body(), *Back_Door->get_rigid_body(), { 0, 6.0f, -3.5f }, { 0,2,0 }, btVector3(1, 0, 0), btVector3(1, 0, 0));
	Back_Door_constrain->setLimit((3 * (3.1416f / 2)), (3.1416f * 2));


	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

btRigidBody * ModulePlayer::GetVehicleBody() const
{
	if (vehicle != NULL)return vehicle->get_rigid_body();
	else return nullptr;
}

void ModulePlayer::ResetPlayer()
{
	vehicle->SetTransform(&App->scene_intro->GetCheckpoint(0));
	checkpoint_num = -1;
	InLap = false;
	engine_current_vol = engine_low_vol;
	chronometer.Start();
	chronometer.Stop();
	vehicle->get_rigid_body()->setLinearVelocity({ 0,0,0 });
	vehicle->get_rigid_body()->setAngularVelocity({ 0,0,0 });
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	vehicle->get_rigid_body()->getWorldTransform().getOpenGLMatrix(&chasis_loc);
	turn = acceleration = brake = 0.0f;
	bool contact = vehicle->vehicle->m_wheelInfo[0].m_raycastInfo.m_isInContact;

	//Car engine ---------
	if (engine_timer.Read() > engine_loop)
	{
		Mix_Volume(2, engine_current_vol);
		if(engine_rpm < 105)App->audio->PlayFx(engine_low_rpm_fx,2);
		else if (engine_rpm < 200)App->audio->PlayFx(engine_mid_rpm_fx,2);
		else if(engine_rpm < 320)App->audio->PlayFx(engine_high_rpm_fx, 2);
		else App->audio->PlayFx(engine_max_rpm_fx, 2);
		engine_timer.Start();
	}
	if (contact)engine_rpm = vehicle->GetKmh();
	// -------------------



	

	//Car Mechanics --------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		if (contact)
		{
			acceleration = MAX_ACCELERATION;
			if(engine_current_vol < engine_high_vol)engine_current_vol+=0.3f;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		if (contact)
		{
			if (vehicle->GetKmh() < 2.5f)acceleration = -MAX_ACCELERATION;
			else brake = BRAKE_POWER;
			if(engine_current_vol > engine_low_vol)engine_current_vol-=0.7f;
		}

		if (vehicle->vehicle_lights[0].color.r < 1.5f && break_timer.Read() > break_rate)
		{
			vehicle->vehicle_lights[0].FadeColor(0.35f, 0, 0);
			vehicle->vehicle_lights[1].FadeColor(0.35f, 0, 0);
			break_timer.Start();
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		vehicle->vehicle_lights[0].color = Gray;
		vehicle->vehicle_lights[1].color = Gray;
	}

	btVector3 chasis_ang_vel = vehicle->get_rigid_body()->getAngularVelocity();

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (contact)
		{
			if (turn < TURN_DEGREES)	turn += TURN_DEGREES;
		}
		else
		{
			vehicle->get_rigid_body()->setAngularVelocity({ chasis_ang_vel.getX(),chasis_ang_vel.getY() + 0.04f,chasis_ang_vel.getZ() });
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (contact)
		{
			if (turn > -TURN_DEGREES)	turn -= TURN_DEGREES;
		}
		else
		{
			vehicle->get_rigid_body()->setAngularVelocity({ chasis_ang_vel.getX(),chasis_ang_vel.getY() - 0.04f,chasis_ang_vel.getZ() });
		}
	}


	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_UP && engine_rpm > 275 && contact && turbo_timer.Read() > turbo_rate)
	{
		App->audio->PlayFx(turbo_fx, 1);
		turbo_timer.Start();
	}

	//Open doors -----------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN)
	{
		door_1->get_rigid_body()->applyCentralForce(btVector3(10000, 0, 0));
		door_2->get_rigid_body()->applyCentralForce(btVector3(10000, 0, 0));
		Back_Door->get_rigid_body()->applyCentralForce(btVector3(0, 0, 30000));
	}

	//Lights ON ------------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
	{
		if (lights_on)
		{
			lights_on = false;
			up_light_on = false;
			vehicle->vehicle_lights[4].color = White;
			vehicle->vehicle_lights[5].color = White;
		}
		else
		{
			lights_on = true;
			up_light_on = true;
		}
	}

	if (lights_on && lights_timer.Read() > lights_rate)
	{
		if (up_light_on)
		{
			vehicle->vehicle_lights[4].FadeColor(0, 0.05f, 0.2);
			vehicle->vehicle_lights[5].FadeColor(0, -0.05f, -0.2);
			if (vehicle->vehicle_lights[4].color.g > 1.5f)up_light_on = false;
		}
		else if(up_light_on == false)
		{
			vehicle->vehicle_lights[4].FadeColor(0, -0.05f, -0.2);
			vehicle->vehicle_lights[5].FadeColor(0, 0.05f, 0.2);
			if (vehicle->vehicle_lights[4].color.g < 0.5f)up_light_on = true;
		}
		lights_timer.Start();
	}

	// Car Jump ------------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		if (contact) {
			mat4x4 trans;
			this->vehicle->GetTransform(&trans);
			vec3 PushVector(trans[4], trans[5], trans[6]);
			PushVector *= 24000;
			vehicle->Push(PushVector.x, PushVector.y, PushVector.z);
			App->audio->PlayFx(hydraulic_suspension_fx);
		}
	}

	//Good Mode ------------------------------------------------
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)god = !god;

	if (god) {

		//Reset the vehicle position
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		{
			App->audio->PlayFx(car_reset_fx);
			ResetPlayer();
			App->scene_intro->ResetCheckpoints();
		}

	}
	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	vehicle->Render();

	char title[80];
	sprintf_s(title, "Current Lap: %i || Record Lap: %i || %.1f Km/h", chronometer.Read(), record, vehicle->GetKmh());
	App->window->SetTitle(title);


	


	door_1->GetTransform(print_door_1.transform.M);
	door_2->GetTransform(print_door_2.transform.M);
	Back_Door->GetTransform(print_Back_Door.transform.M);

	print_door_1.Render();
	print_door_2.Render();
	print_Back_Door.Render();

	return UPDATE_CONTINUE;
}

