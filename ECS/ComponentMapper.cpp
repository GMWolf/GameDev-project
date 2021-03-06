#include "ComponentMapper.h"
#include "Aspect.h"

namespace ECS {


	//Construct On First Use (static init order fiasco)
	std::vector<baseComponentMapper*>* baseComponentMapper::mappers()
	{
		static std::vector<baseComponentMapper*>* m = new std::vector<baseComponentMapper*>(ASPECT_SIZE);
		return m;
	}

	int* baseComponentMapper::getCompCounter()
	{
		static int* i = new int(0);
		return i;
	}

	std::unordered_map<std::string, baseComponentMapper*>* baseComponentMapper::mappersByName()
	{
		static std::unordered_map<std::string, baseComponentMapper*>* m = new std::unordered_map<std::string, baseComponentMapper*>();
		return m;
	}
}