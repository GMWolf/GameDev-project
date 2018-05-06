#include "MovementSystem.h"
#include "SubscriptionManager.h"
#include "Transform.h"
#include "DeltaTime.h"
#include <iostream>
#include "VelocitySystem.h"
#include "Lights.h"
#include "SystemManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "MeshFilter.h"
#include "LightFade.h"
#include "ECS.h"
#include "CommonComponentLoader.h"

PlayerControlSystem::PlayerControlSystem():
	playerControled(ECS::SubscriptionManager::getSubscription(ECS::Aspect::getAspect<PlayerControl, Transform>())), ui(nullptr),
	physics(nullptr)
{
}

PlayerControlSystem::~PlayerControlSystem()
{
}

void PlayerControlSystem::init()
{
	ECS::registerLoader<PlayerControl>(new CommonComponentLoader<PlayerControl>());
	ui = ECS::SystemManager::getSystem<UISystem>();

	physics = ECS::SystemManager::getSystem<PhysicsSystem>();
	physics->registerTickCallback(this);
}

void PlayerControlSystem::update()
{
}

void PlayerControlSystem::end()
{
}

void PlayerControlSystem::tickCallback(btScalar timestep)
{
}

void PlayerControlSystem::pretickCallback(btScalar timestep)
{
	for (ECS::Entity e : playerControled)
	{
		const bool grounded = onGround(e);

		PlayerControl& pc = e.get<PlayerControl>();

		Input* horizontal = ui->getInput(pc.horizontal);
		Input* vertical = ui->getInput(pc.vertical);
		Input* jump = ui->getInput("Jump");

		Transform& t = e.get<Transform>();
		btRigidBody* rb = e.get<RigidBody>().rigidBody;

		if (grounded) {
			btVector3 targetVelocity;
			targetVelocity += btVector3(t.rotation[2].x, t.rotation[2].y, t.rotation[2].z) * (*vertical)();
			targetVelocity += btVector3(t.rotation[0].x, t.rotation[0].y, t.rotation[0].z) * (*horizontal)();
			if (targetVelocity.norm() > 1)
			{
				targetVelocity.normalize();
			}
			targetVelocity *= pc.speed;


			const btVector3 velocity = rb->getVelocityInLocalPoint(btVector3(0, 0, 0));
			btVector3 velocityDelta = targetVelocity - velocity;
			velocityDelta.setY(0);
			const float l = velocityDelta.norm();
			if (l > pc.maxChange)
			{
				velocityDelta.normalize();
				velocityDelta *= pc.maxChange;
			}

			btVector3 force = (velocityDelta / rb->getInvMass()) / timestep;
			if (force.norm() > pc.maxForce)
			{
				force.normalize();
				force *= pc.maxForce;
			}

			rb->activate(true);
			rb->applyCentralForce(force);

			if ((*jump)())
			{
				rb->applyCentralImpulse(btVector3(0, 45, 0));
			}
			
		}
		

	}
}

bool PlayerControlSystem::onGround(ECS::Entity e)
{
	Transform& t = e.get<Transform>();
	RigidBody& rb = e.get<RigidBody>();

	btVector3 aabbmin, aabbmax;
	rb.rigidBody->getAabb(aabbmin, aabbmax);

	glm::vec3 feet(t.position.x, aabbmax.y(), t.position.z);

	PhysicsSystem::Hit hit;
	physics->RayCastClosest(t.position, t.position - glm::vec3(0,2, 0), hit);

	return hit.hasHit;
}