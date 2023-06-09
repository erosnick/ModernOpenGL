#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "Texture.h"

namespace AriaRenderer 
{
	class ComputeShader
	{
	public:
		ComputeShader();
		ComputeShader(const std::string& path);

		void initialize(const glm::uvec2& inWorkSize, const glm::uvec2& inTextureSize);

		void load(const std::string& path);

		void use();
		void dispatch();
		void wait();

		void prepareComputeResources();

		std::vector<float> getComputeData();

		uint32_t getTexture() const { return computeTexture.getId(); }

	private:
		void checkCompileErrors(uint32_t shader, std::string type);

	private:
		unsigned int ID;
		glm::uvec2 workSize;
		glm::uvec2 textureSize;

		Texture computeTexture;
	};
}