#include "stdafx.h"
#include "Renderer.h"
#include <iostream>
#include <Vector3.h>
#include <DeltaTime.h>
#include <ShaderProgramLoader.h>
#include <ImageLoader.h>
#include "meshLoader.h"
#include <Transform.h>
#include "MeshFilter.h"
#include "Lights.h"
#include "Camera.h"

Renderer::Renderer(int width, int height)
	: width(width), height(height),
	  renderEntities(SubscriptionManager::getSubscription(Aspect::getAspect<Transform, MeshFilter>())),
	  lights(SubscriptionManager::getSubscription(Aspect::getAspect<Transform, PointLight>())), 
	  camera(SubscriptionManager::getSubscription(Aspect::getAspect<Camera>()))
{
}


Renderer::~Renderer()
{
}

void Renderer::init()
{
	//glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	GenerateFBO();

	geometryProgram = ShaderProgramLoader::Load("shaders/geometry.shd");
	
	resolveProgram = ShaderProgramLoader::Load("shaders/simpleDiffuse.shd");
	resolveProgram->Getuniform("screenSize") = Vector2(width, height);
	
	lightProgram = ShaderProgramLoader::Load("shaders/light.shd");
	lightProgram->Getuniform("screenSize") = Vector2(width, height);

	quad = new Mesh(Mesh::Quad(Vector3(-1, -1, 0), Vector3(1, -1, 0), Vector3(1, 1, 0), Vector3(-1, 1, 0),
		Vector2(0, 0), Vector2(0, 1), Vector2(1, 1), Vector2(0, 1),
		Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0)));

	/*texture->bind(0);
	geometryProgram->Getuniform("diffuseTex") = 0;*/

	projection = Matrix4::Perspective(0.01, 1000, width / ((float) height), 60);
	view = Matrix4::ViewMatrix(Vector3(0, 0, 3), Vector3(0, 0, 0), Vector3(0, 1, 0));
}

void Renderer::update()
{
	/*Vector3 pos = view.position;
	bool left = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
	if (left) {
		pos += (view.left * DeltaTime::delta * 2);
	}
	bool right = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
	if (right) {
		pos += -(view.left * DeltaTime::delta * 2);
	}
	bool up = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	if (up) {
		pos += -(view.up * DeltaTime::delta * 2);
	}
	bool down = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
	if (down) {
		pos += (view.up * DeltaTime::delta * 2);
	}
	bool forward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
	if (forward) {
		pos += (view.forward * DeltaTime::delta * 2);
	}
	bool backward = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
	if (backward) {
		pos += -(view.forward * DeltaTime::delta * 2);
	}
	view.position = pos.xyz;*/

	Entity camEntity = camera.getFirst();
	view = camEntity.get<Camera>().view;

	render();
}

void Renderer::render() const
{
	glClearColor(0, 0, 0, 0);

	geometryPass();
	lightPass();
	resolvePass();
	
	geometryBuffer->blit(GL_COLOR_ATTACHMENT0, 0, 0, width, height, 0, 0, width / 4, height / 4, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	lightBuffer->blit(GL_COLOR_ATTACHMENT0, 0, 0, width, height, width / 4, 0, width /2, height / 4, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void Renderer::end()
{
}

void Renderer::GenerateFBO()
{
	geometryBuffer = new wagl::FrameBuffer(width, height);
	normalTexture = geometryBuffer->createTexture(GL_RGB16F, GL_RGB, GL_HALF_FLOAT, GL_COLOR_ATTACHMENT0, true);
	depthTexture = geometryBuffer->createTexture(GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_HALF_FLOAT, GL_DEPTH_ATTACHMENT, false);
	
	GLenum status = geometryBuffer->status();
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "geometry FB error, status:" << status << std::endl;
		return;
	}
	std::cout << "geometry FB all good" << std::endl;

	lightBuffer = new wagl::FrameBuffer(width, height);
	lightTexture = lightBuffer->createTexture(GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_COLOR_ATTACHMENT0, true);

	status = lightBuffer->status();
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "light FB error, status:" << status << std::endl;
		return;
	}
	std::cout << "light FB all good" << std::endl;


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::geometryPass() const
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	//Geometry pass
	geometryBuffer->bindDraw();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	geometryProgram->use();

//	geometryProgram->Getuniform("normalTex") = 0;

	const Matrix4 mv = projection * view;
	for (Entity e : renderEntities) {
		const Matrix4 model = e.get<Transform>().getMatrix();
		geometryProgram->Getuniform("MVP") = mv * model ;
		geometryProgram->Getuniform("model") = model;
		e.get<MeshFilter>().normal->bind(0);
		//std::cout << e.getId() << std::endl;
		e.get<MeshFilter>().mesh->draw();
	}
}

void Renderer::lightPass() const
{

	lightMesh.clear();

	for(Entity light : lights)
	{
		Transform& t = light.get<Transform>();
		PointLight& pl = light.get<PointLight>();

		lightMesh.addLight(t.position, pl.colour, pl.radius);
	}

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	//Light pass
	lightBuffer->bindDraw();
	glClear(GL_COLOR_BUFFER_BIT);

	//lightProgram->Getuniform("view") = view;
	lightProgram->Getuniform("MVP") = projection * view;
	lightProgram->Getuniform("invView") = view.inverse();
	lightProgram->Getuniform("invProjection") = projection.inverse();

	depthTexture->bind(0);
	lightProgram->Getuniform("depthTex") = 0;

	normalTexture->bind(2);
	lightProgram->Getuniform("normalTex") = 2;
	
	lightProgram->use();

	lightMesh.draw();
}

void Renderer::resolvePass() const
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	//Final pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	lightTexture->bind(0);
	resolveProgram->Getuniform("lightTex") = 0;

	//texture->bind(1);
	resolveProgram->Getuniform("diffuseTex") = 1;

	resolveProgram->use();
	const Matrix4 mv = projection * view;
	for (Entity e : renderEntities) {
		const Matrix4 model = e.get<Transform>().getMatrix();
		resolveProgram->Getuniform("MVP") = mv * model;
		resolveProgram->Getuniform("model") = model;
		//std::cout << e.getId() << '\n';
		e.get<MeshFilter>().texture->bind(1);
		e.get<MeshFilter>().mesh->draw();
	}
}
