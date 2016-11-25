#include "PhysBody3D.h"
#include "glmath.h"
#include "Engine/Bullet/include/btBulletDynamicsCommon.h"


// ---------------------------------------------------------
PhysBody3D::PhysBody3D(btRigidBody* body):body(body)
{}

// ---------------------------------------------------------
PhysBody3D::~PhysBody3D()
{}

// ---------------------------------------------------------
void PhysBody3D::GetTransform(float* matrix) const
{
	//Get the PhysBody3D transformation matrix
	if (body != NULL && matrix != NULL) {
		body->getWorldTransform().getOpenGLMatrix(matrix);
	}
}

// ---------------------------------------------------------
void PhysBody3D::SetTransform(const float* matrix) const
{
	//Set the PhysBody3D transformation matrix
	if (body != NULL && matrix != NULL) {
		btTransform transform;
		transform.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(transform);
	}
}

// ---------------------------------------------------------
void PhysBody3D::SetPos(float x, float y, float z)
{
	//Set the position of the body
	if (body != NULL) {
		btTransform trans = body->getWorldTransform();
		trans.setOrigin(btVector3(x, y, z));
		body->setWorldTransform(trans);
	}
}

btRigidBody* PhysBody3D::get_rigid_body() const
{
	return body;
}