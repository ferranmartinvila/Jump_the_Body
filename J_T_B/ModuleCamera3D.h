#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

#define Z_DIST 26

struct PhysVehicle3D;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();

private:

	void CalculateViewMatrix();

	float camera_speed;

public:
	
	vec3 X, Y, Z, Position, Reference;

private:

	PhysVehicle3D*	Target;
	vec3			CameraLocation;
	vec3			ViewVector;
	vec3			VehicleLocation;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};