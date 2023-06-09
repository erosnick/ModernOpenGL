#include "AriaPCH.h"

#include "ComputeShader.h"

#include <glad/glad.h>

namespace AriaRenderer
{
	ComputeShader::ComputeShader()
		: ID(), workSize{}
	{
	}

	ComputeShader::ComputeShader(const std::string& path)
	{
		load(path);
	}

	void ComputeShader::initialize(const glm::uvec2& inWorkSize, const glm::uvec2& inTextureSize)
	{
		workSize = inWorkSize;
		textureSize = inTextureSize;
	}

	void ComputeShader::load(const std::string& path)
	{
		// 1. retrieve the vertex/fragment source code from filePath
		std::string computeShaderString;
		std::ifstream computeShaderFile;

		// ensure ifstream objects can throw exceptions:
		computeShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			// open files
			computeShaderFile.open(path);
			std::stringstream computeShaderStream;
			// read file's buffer contents into streams
			computeShaderStream << computeShaderFile.rdbuf();
			// close file handlers
			computeShaderFile.close();
			// convert stream into string
			computeShaderString = computeShaderStream.str();
		}

		catch (std::ifstream::failure& e)
		{
			ARIA_CORE_CRITICAL("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: {0}", e.what());
		}

		// 2. compile shaders

		// compute shader
		const char* computeShaderCode = computeShaderString.c_str();
		unsigned int computeShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(computeShader, 1, &computeShaderCode, NULL);
		glCompileShader(computeShader);
		checkCompileErrors(computeShader, "COMPUTE");

		// shader Program
		ID = glCreateProgram();

		glAttachShader(ID, computeShader);

		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(computeShader);
	}

	void ComputeShader::use()
	{
		glUseProgram(ID);
		computeTexture.use(0);
	}

	void ComputeShader::dispatch()
	{
		// Just keep it simple, 2d work group
		//glDispatchCompute(workSize.x, workSize.y, 1);
		glDispatchCompute(workSize.x, workSize.y, 1);
	}

	void ComputeShader::wait()
	{
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

	void ComputeShader::prepareComputeResources()
	{
		computeTexture.createImage(textureSize.x, textureSize.y);
	}

	std::vector<float> ComputeShader::getComputeData()
	{
		return computeTexture.getImageData();
	}

	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void ComputeShader::checkCompileErrors(uint32_t shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				ARIA_CORE_CRITICAL("ERROR::SHADER_COMPILATION_ERROR of type: {0}\n{1}\n{2}", type, infoLog, " -- --------------------------------------------------- -- ");
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				ARIA_CORE_CRITICAL("ERROR::PROGRAM_LINKING_ERROR of type: {0}\n{1}\n{2}", type, infoLog, " -- --------------------------------------------------- -- ");
			}
		}
	}
}