#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "PhysBody3D.h"
#include "Primitive.h"

#ifdef _DEBUG
#pragma comment (lib, "Bullet/libx86/BulletDynamics_debug.lib")
#pragma comment (lib, "Bullet/libx86/BulletCollision_debug.lib")
#pragma comment (lib, "Bullet/libx86/LinearMath_debug.lib")
#else
#pragma comment (lib, "Bullet/libx86/BulletDynamics.lib")
#pragma comment (lib, "Bullet/libx86/BulletCollision.lib")
#pragma comment (lib, "Bullet/libx86/LinearMath.lib")
#endif

ModulePhysics3D::ModulePhysics3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = false;

	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	debug_draw = new DebugDrawer();
}

// Destructor
ModulePhysics3D::~ModulePhysics3D()
{
	delete debug_draw;
	delete solver;
	delete broad_phase;
	delete dispatcher;
	delete collision_conf;
}

// Render not available yet----------------------------------
bool ModulePhysics3D::Init()
{
	LOG("Creating 3D Physics simulation");
	bool ret = true;

	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics3D::Start()
{
	LOG("Creating Physics environment");

	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	world->setDebugDrawer(debug_draw);
	world->setGravity(GRAVITY);

	// Big rectangle as ground
	{
		btCollisionShape* colShape = new btBoxShape(btVector3(100.0f, 2.0f, 100.0f));

		btDefaultMotionState* myMotionState = new btDefaultMotionState();
		btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);

		btRigidBody* body = new btRigidBody(rbInfo);
		world->addRigidBody(body);
	}

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PreUpdate(float dt)
{
	world->stepSimulation(dt, 15);

	// TODO 6: Detect collisions:
	uint manifolds_num = world->getDispatcher()->getNumManifolds();
	// - Iterate all manifolds
	for (uint k = 0; k < manifolds_num; k++) {
		// - Count the number of contacts
		btPersistentManifold* current_mainfold = world->getDispatcher()->getManifoldByIndexInternal(k);
		if ((current_mainfold->getNumContacts()) > 0) {
			// - If we have contacts, get both PhysBody3D from userpointers
			PhysBody3D* bodyA =  ((PhysBody3D*)current_mainfold->getBody0()->getUserPointer());
			PhysBody3D* bodyB = ((PhysBody3D*)current_mainfold->getBody1()->getUserPointer());
			// - If iterate all contact listeners and call them
			if (bodyA != NULL && bodyB != NULL) {
				
				uint listeneres_num = bodyA->collision_listeners.count();
				Module* current_listener = nullptr;
				for (uint g = 0; g < listeneres_num; g++) {
					bodyA->collision_listeners.at(g, current_listener);
					current_listener->OnCollision(bodyA,bodyB);
				}
				
				listeneres_num = bodyB->collision_listeners.count();
				current_listener = nullptr;
				for (uint g = 0; g < listeneres_num; g++) {
					bodyB->collision_listeners.at(g, current_listener);
					current_listener->OnCollision(bodyA, bodyB);
				}
			}
		}
	}
	

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::Update(float dt)
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(debug == true)
	{
		world->debugDrawWorld();

		if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			Sphere s(1);
			s.SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
			AddBody(s);
		}

		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		{
			Cube c(0.5f,0.5f,0.5f);
			c.SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
			AddBody(c);
		}
	}

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics3D::CleanUp()
{
	LOG("Destroying 3D Physics simulation");

	// Remove from the world all collision bodies
	for(int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	// TODO 2: Free all collision shapes and bodies
	
	//Free the shapes
	p2List_item<btCollisionShape*>* temp = shapes.getFirst();
	p2List_item<btCollisionShape*>* temp_next;
	if(temp) temp_next = shapes.getFirst()->next;
	while (temp != NULL) {
		shapes.del(temp);
		temp = temp_next;
		if(temp_next)temp_next = temp_next->next;
	}
	shapes.clear();
	LOG("Shapes Cleared!\n");

	//Free the bodies
	p2List_item<PhysBody3D*>* body_temp = bodies.getFirst();
	p2List_item<PhysBody3D*>* body_temp_next;
	if(body_temp)body_temp_next = bodies.getFirst()->next;
	while (body_temp != NULL) {
		bodies.del(body_temp);
		body_temp = body_temp_next;
		if(body_temp_next)body_temp_next = body_temp_next->next;
		
	}
	bodies.clear();
	LOG("Bodies Cleared!\n");

	delete world;

	return true;
}

// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const Sphere& sphere, float mass)
{
	//Create and store body shape
	btCollisionShape* colShape = new btSphereShape(sphere.radius);
	shapes.add(colShape);

	//Set body transform matrix
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&sphere.transform);

	//Calculate and set inertia of object have mass
	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	//Set the body motion state (used later to interpolate and sync body transforms)
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	//Create and store the body
	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);
	bodies.add(pbody);

	//Put body pointers in user & world data
	body->setUserPointer(pbody);
	world->addRigidBody(body);

	return pbody;
}

PhysBody3D * ModulePhysics3D::AddBody(const Cube& cube, float mass)
{
	//Create and store body shape
	btCollisionShape* colShape = new btBoxShape(btVector3 (cube.size.x/2.0f, cube.size.y / 2.0f, cube.size.z / 2.0f));
	shapes.add(colShape);

	//Set body transform matrix
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cube.transform);

	//Calculate and set inertia of object have mass
	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	//Set the body motion state (used later to interpolate and sync body transforms)
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	//Create and store the body
	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);
	bodies.add(pbody);

	//Put body pointers in user & world data
	body->setUserPointer(pbody);
	world->addRigidBody(body);

	return pbody;
}

// =============================================
void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	line.origin.Set(from.getX(), from.getY(), from.getZ());
	line.destination.Set(to.getX(), to.getY(), to.getZ());
	line.color.Set(color.getX(), color.getY(), color.getZ());
	line.Render();
}

void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	point.transform.translate(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
	point.color.Set(color.getX(), color.getY(), color.getZ());
	point.Render();
}

void DebugDrawer::reportErrorWarning(const char* warningString)
{
	LOG("Bullet warning: %s", warningString);
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	LOG("Bullet draw text: %s", textString);
}

void DebugDrawer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes) debugMode;
}

int	 DebugDrawer::getDebugMode() const
{
	return mode;
}
