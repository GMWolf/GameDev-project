#include "LightFadeSystem.h"
#include "SubscriptionManager.h"
#include "Lights.h"
#include "LightFade.h"
#include "Entity.h"
#include "DeltaTime.h"


LightFadeSystem::LightFadeSystem() :
	entities(SubscriptionManager::getSubscription(Aspect::getAspect<PointLight, LightFade>()))
{
}


LightFadeSystem::~LightFadeSystem()
{
}

void LightFadeSystem::init()
{
}

void LightFadeSystem::update()
{
	for(Entity e : entities)
	{
		e.get<PointLight>().power -= e.get<LightFade>().fadeRate * wagl::DeltaTime::delta;
		if (e.get<PointLight>().power <= 0)
		{
			e.remove<PointLight>();
			e.remove<LightFade>();
		}
	}
}

void LightFadeSystem::end()
{
}
