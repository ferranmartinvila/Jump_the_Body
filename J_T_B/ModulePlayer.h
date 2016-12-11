#ifndef _MODULE_PLAYER_
#define _MODULE_PLAYER_
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "ModulePhysics3D.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 8000.0f
#define TURN_DEGREES 25.0f * DEGTORAD
#define BRAKE_POWER 1000.0f

class ModulePlayer : public Module
{
public:
	
	ModulePlayer(Application* app, bool start_enabled = true);
	~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();


	bool god = false;
	uint checkpoint_num = 0;

public:

	PhysVehicle3D* vehicle;
	PhysBody3D* cabine;
	PhysBody3D* door_1;
	PhysBody3D* door_2;
	PhysBody3D* Back_Door;
	PhysBody3D* Back;
	PhysBody3D* Back_1;
	PhysBody3D* Back_2;
	PhysBody3D* roof;

	Cube print_cabine;
	Cube print_door_1;
	Cube print_door_2;
	Cube print_Back_Door;
	Cube print_Back;
	Cube print_Back_1;
	Cube print_Back_2;
	Cube print_roof;

	btHingeConstraint* cabine_to_vehicle;
	btHingeConstraint* door_1_constrain;
	btHingeConstraint* door_2_constrain;
	btHingeConstraint* Back_Door_constrain;
	btHingeConstraint* Back_constrain;
	btHingeConstraint* Back_1_constrain;
	btHingeConstraint* Back_2_constrain;
	btHingeConstraint* roof_constrain;

	float turn;
	float acceleration;
	float brake;

public:

	uint hydraulic_suspension_fx;

public:

	btRigidBody*		GetVehicleBody()const;

};

#endif;