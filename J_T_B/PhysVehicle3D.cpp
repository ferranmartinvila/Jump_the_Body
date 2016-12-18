#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "ModulePlayer.h"
#include "Application.h"
#include "Bullet/include/btBulletDynamicsCommon.h"
#include "Bullet\include\btBulletDynamicsCommon.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != NULL)
	//delete wheels;
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
	Cylinder wheel;
	wheel.color = Blue;

	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel.radius = info.wheels[0].radius;
		wheel.height = info.wheels[0].width;

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);

		wheel.Render();
	}

	Cube chassis(info.chassis_size.x, 1, info.chassis_size.z);
	Cube Cabine(6, 2, 1);
	Cube print_Back_1(1, 4, 3);
	Cube print_Back_2(1, 4, 3);
	Cube roof(6, 0.25f, 8);

	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&Cabine.transform);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&print_Back_1.transform);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&print_Back_2.transform);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&roof.transform);

	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();

	btVector3 offset(info.chassis_offset.x, 1.5, info.chassis_offset.z);
	btVector3 cabine_position(0, 2.5f, 3.5f);
	btVector3 print_Back_1_position(2.5f, 4, -2.5f);
	btVector3 print_Back_2_position(-2.5f, 4, -2.5f);
	btVector3 roof_position(0, 6.0f, 0);

	offset = offset.rotate(q.getAxis(), q.getAngle());
	cabine_position = cabine_position.rotate(q.getAxis(), q.getAngle());
	print_Back_1_position = print_Back_1_position.rotate(q.getAxis(), q.getAngle());
	print_Back_2_position = print_Back_2_position.rotate(q.getAxis(), q.getAngle());
	roof_position = roof_position.rotate(q.getAxis(), q.getAngle());

	chassis.transform.M[12] += offset.getX();
	chassis.transform.M[13] += offset.getY();
	chassis.transform.M[14] += offset.getZ();

	Cabine.transform.M[12] += cabine_position.getX();
	Cabine.transform.M[13] += cabine_position.getY();
	Cabine.transform.M[14] += cabine_position.getZ();

	print_Back_1.transform[12] += print_Back_1_position.getX();
	print_Back_1.transform[13] += print_Back_1_position.getY();
	print_Back_1.transform[14] += print_Back_1_position.getZ();

	print_Back_2.transform[12] += print_Back_2_position.getX();
	print_Back_2.transform[13] += print_Back_2_position.getY();
	print_Back_2.transform[14] += print_Back_2_position.getZ();

	roof.transform[12] += roof_position.getX();
	roof.transform[13] += roof_position.getY();
	roof.transform[14] += roof_position.getZ();


	Cabine.Render();
	print_Back_2.Render();
	print_Back_1.Render();
	roof.Render();
	chassis.Render();
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}