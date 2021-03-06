#include "UISystem.h"
#include "DeltaTime.h"
#include <MathUtils.h>
#include "json.hpp"
#include <fstream>
#include <iostream>

std::unordered_map<std::string, Keys> keysByName =
{
	{ "KEY_A", KEY_A },
	{ "KEY_B", KEY_B },
	{ "KEY_C", KEY_C },
	{ "KEY_D", KEY_D },
	{ "KEY_E", KEY_E },
	{ "KEY_F", KEY_F },
	{ "KEY_G", KEY_G },
	{ "KEY_H", KEY_H },
	{ "KEY_I", KEY_I },
	{ "KEY_J", KEY_J },
	{ "KEY_K", KEY_K },
	{ "KEY_L", KEY_L },
	{ "KEY_M", KEY_M },
	{ "KEY_N", KEY_N },
	{ "KEY_O", KEY_O },
	{ "KEY_P", KEY_P },
	{ "KEY_Q", KEY_Q },
	{ "KEY_R", KEY_R },
	{ "KEY_S", KEY_S },
	{ "KEY_T", KEY_T },
	{ "KEY_U", KEY_U },
	{ "KEY_V", KEY_V },
	{ "KEY_W", KEY_W },
	{ "KEY_X", KEY_X },
	{ "KEY_Y", KEY_Y },
	{ "KEY_Z", KEY_Z },
	{ "KEY_ESCAPE", KEY_ESCAPE },
	{ "KEY_LEFT_SHIFT", KEY_LEFT_SHIFT },
	{ "KEY_RIGHT_SHIFT", KEY_RIGHT_SHIFT },
	{ "KEY_LEFT_CTRL", KEY_LEFT_CTRL },
	{ "KEY_RIGHT_CTRL", KEY_RIGHT_CTRL },
	{ "KEY_UP", KEY_UP },
	{ "KEY_DOWN", KEY_DOWN },
	{ "KEY_LEFT", KEY_LEFT },
	{ "KEY_RIGHT", KEY_RIGHT },
	{ "KEY_SPACE", KEY_SPACE },
	{ "KEY_ALT", KEY_ALT},
	{"KEY_GRAVE", KEY_GRAVE}
};

std::unordered_map<std::string, Buttons> buttonsByName {
	{"BUTTON_LEFT", BUTTON_LEFT},
	{"BUTTON_RIGHT", BUTTON_RIGHT}
};


UISystem::UISystem(GLFWwindow* window, wagl::ApplicationAdapter* app) : window(window), app(app)
{
}


UISystem::~UISystem()
{
}

void UISystem::init()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	double mx, my;
	glfwGetCursorPos(window, &mx, &my);
	mousePrevious.x = mx;
	mousePrevious.y = my;

	loadInputs("inputs.json");
	
}

void UISystem::update()
{
	if (glfwGetKey(window, KEY_ESCAPE))
	{
		app->ShouldClose = true;
	}

	double mx, my;
	glfwGetCursorPos(window, &mx, &my);
	mousePos.x = mx;
	mousePos.y = my;

	if (!getKey(KEY_ALT))
	{
		mouseDelta = mousePos - mousePrevious;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		
	} else
	{
		mouseDelta.x = 0;
		mouseDelta.y = 0;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	mousePrevious = mousePos;

	for(Input* input : inputs)
	{
		input->update(*this, wagl::DeltaTime::delta);
	}
}

void UISystem::end()
{
}

bool UISystem::getKey(Keys key) const
{
	return glfwGetKey(window, key) == GLFW_PRESS;
}

bool UISystem::getMouseButton(Buttons button) const
{
	return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

glm::vec2 UISystem::getMousePos() const
{
	return mousePos;
}

glm::vec2 UISystem::getMouseDelta() const
{
	return mouseDelta;
}

Input* UISystem::getInput(std::string name)
{
	if (inputsByname.find(name) == inputsByname.end()) {
		std::cout << "Unknown input " << name << std::endl;
		return new DummyInput;
	}

	return inputsByname[name];
}

void UISystem::addInput(std::string name, Input* input)
{
	inputs.push_back(input);
	inputsByname.insert({ name, input });
}

void UISystem::addInput(nlohmann::json& js)
{
	const std::string name = js["name"];
	const std::string type = js["type"];

	if (type == "axis")
	{
		addInput(name, new AxisInput(js));
	}
	if (type == "multiplexed")
	{
		addInput(name, new MultiplexedInput(*this, js));
	}
	if (type == "mouse button")
	{
		addInput(name, new MouseButtonInput(js));
	}
}

bool UISystem::hasInput(std::string name)
{
	return inputsByname.find(name) != inputsByname.end();
}

void UISystem::loadInputs(std::string file)
{
	std::ifstream in("inputs.json");
	nlohmann::json j;
	in >> j;

	for(auto& js : j)
	{
		addInput(js);
	}

}

float DummyInput::operator()()
{
	return 0;
}

void DummyInput::update(UISystem& ui, float dt)
{
}

AxisInput::AxisInput(Keys positive, Keys negative, float acc, float rest): positiveKey(positive), negativeKey(negative),
                                                                           acceleration(acc), restitution(rest)
{
}

AxisInput::AxisInput(nlohmann::json js)  
{
	positiveKey = keysByName[js["positive"].get<std::string>()];
	negativeKey = keysByName[js["negative"].get<std::string>()];

	acceleration = js["acceleration"].get<float>();
	restitution = js["restitution"].get<float>();
}

float AxisInput::operator()()
{
	return value;
}

void AxisInput::update(UISystem& ui, float dt)
{
	int i = ui.getKey(positiveKey) - ui.getKey(negativeKey);
	
	if (i != 0)
	{
		value += acceleration * i * dt;
	}
	else
	{
		value -= sgn(value) * min(restitution * dt, abs(value));
	}


	value = clamp(value, -1.f, 1.f);
}

ButtonInput::ButtonInput(Keys key): key(key)
{
}

float ButtonInput::operator()()
{
	return value;
}

void ButtonInput::update(UISystem& ui, float dt)
{
	value = ui.getKey(key);
}

MouseDeltaInput::MouseDeltaInput(bool hor): horizontal(hor)
{
}

float MouseDeltaInput::operator()()
{
	return value;
}

void MouseDeltaInput::update(UISystem& ui, float dt)
{
	value = horizontal ? ui.getMouseDelta().x : ui.getMouseDelta().y;
}

MouseButtonInput::MouseButtonInput(Buttons b): button(b), value(false)
{
}

MouseButtonInput::MouseButtonInput(nlohmann::json js): value(false)
{
	button = buttonsByName[js["button"].get<std::string>()];
}

float MouseButtonInput::operator()()
{
	return value;
}

void MouseButtonInput::update(UISystem& ui, float dt)
{
	value = ui.getMouseButton(button);
}

MultiplexedInput::MultiplexedInput(std::initializer_list<Input*> ins)
{
	for (Input* input : ins)
	{
		inputs.emplace_back(input);
	}
}

MultiplexedInput::MultiplexedInput(UISystem& ui, nlohmann::json& js)
{
	for (auto& e : js["inputs"])
	{
		std::string name = e.get<std::string>();

		inputs.emplace_back(ui.getInput(name));
	}
}

float MultiplexedInput::operator()()
{
	return value;
}

void MultiplexedInput::update(UISystem& ui, float dt)
{
	float absMax = 0;
	float best = 0;
	for (Input* i : inputs)
	{
		float v = (*i)();
		if (abs(v) > absMax)
		{
			best = v;
			absMax = abs(v);
		}
	}


	value = best;
}
