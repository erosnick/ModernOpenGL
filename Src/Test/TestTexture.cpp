#include "AriaPCH.h"

#include "TestTexture.h"

#include <glad/glad.h>

#include <imgui.h>

#include "Renderer/Vertex.h"

#include <glm/gtc/matrix_transform.hpp>

TestTexture::TestTexture()
{
	// initialize (if necessary)
	std::vector<SimpleVertex> vertices = 
	{
		// back face
		{-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f }, // bottom-left
		{ 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f }, // top-right
		{ 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f }, // bottom-right         
		{ 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f }, // top-right
		{-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f }, // bottom-left
		{-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f }, // top-left
		// front face
		{-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f }, // bottom-left
		{ 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f }, // bottom-right
		{ 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f }, // top-right
		{ 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f }, // top-right
		{-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f }, // top-left
		{-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f }, // bottom-left
		// left face
		{-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f }, // top-right
		{-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f }, // top-left
		{-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f }, // bottom-left
		{-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f }, // bottom-left
		{-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f }, // bottom-right
		{-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f }, // top-right
		// right face
		{1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f }, // top-left
		{1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f }, // bottom-right
		{1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f }, // top-right         
		{1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f }, // bottom-right
		{1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f }, // top-left
		{1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f }, // bottom-left     
		 // bottom face
		{-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f }, // top-right
		{ 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f }, // top-left
		{ 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f }, // bottom-left
		{ 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f }, // bottom-left
		{-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f }, // bottom-right
		{-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f }, // top-right
		 // top face
		{-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f }, // top-left
		{ 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f }, // bottom-right
		{ 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f }, // top-right  
		{ 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f }, // bottom-right
		{-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f }, // top-left
		{-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f }  // bottom-left
	};

	std::vector<uint32_t> indices;

	for (uint32_t i = 0; i < 36; i++)
	{
		indices.push_back(i);
	}

	mesh.vertices = vertices;
	mesh.numVertices = 36;
	mesh.indices = indices;
	mesh.numIndices = 36;

	mesh.createBuffers();

	texture = Texture::load("Assets/Textures/Kanna.jpg");
}

TestTexture::~TestTexture()
{
}

void TestTexture::onUpdate(float deltaTime)
{

}

void TestTexture::onRender(const Shader& shader)
{
	auto localShader = shader;

	GLfloat clearColor[]{ 0.4f, 0.6f, 0.9f, 1.0f };
	glClearNamedFramebufferfv(0, GL_COLOR, 0, clearColor);

	GLfloat depth[]{ 1.0f, 1.0f, 1.0f, 1.0f };
	glClearNamedFramebufferfv(0, GL_DEPTH, 0, depth);

	mesh.use();

	localShader.use();

	texture.use();

	auto model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	localShader.setMat4("model", model);

	//auto view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//localShader.setMat4("view", view);

	//auto projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.1f, 100.0f);

	//localShader.setMat4("projection", projection);

	glDrawElements(GL_TRIANGLES, mesh.numIndices, GL_UNSIGNED_INT, nullptr);
}

void TestTexture::onImGuiRender()
{
}
