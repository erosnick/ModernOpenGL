#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Log.h"

struct ShaderSource
{
	std::string vertexShaderSource;
	std::string fragmentShaderSource;
	std::string geometryShaderSource;
};

enum class ShaderType : int8_t
{
	None = -1,
	Vertex = 0,
	Fragment = 1,
	Geometry = 2,
	TessellationControl = 3,
	TessellationEvaluation = 4
};

class Shader
{
public:
	GLuint ID;
	GLuint pipeline;
    
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader()
    {}

    Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = std::string())
    {
        load(vertexPath, fragmentPath, geometryPath);
    }

	ShaderSource parseShader(const std::string& path)
	{
		ShaderSource shaderSource;

		std::ifstream shaderFile(path);
		
		std::string line;
		std::stringstream result[3];

		ShaderType shaderType = ShaderType::None;

		while (getline(shaderFile, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
				{
					shaderType = ShaderType::Vertex;
				}
				else if (line.find("fragment") != std::string::npos)
				{
					shaderType = ShaderType::Fragment;
				}
				else if (line.find("geometry") != std::string::npos)
				{
					shaderType = ShaderType::Geometry;
				}
			}
			else
			{
				result[static_cast<int32_t>(shaderType)] << line << '\n';
			}
		}

		return { result[0].str(), result[1].str(), result[2].str() };
	}

    void load(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = std::string())
    {
		// 1. retrieve the vertex/fragment source code from filePath
		std::string vertexShaderCode;
		std::string fragmentShaderCode;
		std::string geometryShaderCode;
		std::ifstream vertexShaderFile;
		std::ifstream fragmentShaderFile;
		std::ifstream geometryShaderFile;

		// ensure ifstream objects can throw exceptions:
		vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		geometryShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			// open files
			vertexShaderFile.open(vertexPath);
			fragmentShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vertexShaderFile.rdbuf();
			fShaderStream << fragmentShaderFile.rdbuf();
			// close file handlers
			vertexShaderFile.close();
			fragmentShaderFile.close();
			// convert stream into string
			vertexShaderCode = vShaderStream.str();
			fragmentShaderCode = fShaderStream.str();

			// if geometry shader path is present, also load a geometry shader
			if (!geometryPath.empty())
			{
				geometryShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << geometryShaderFile.rdbuf();
				geometryShaderFile.close();
				geometryShaderCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure& e)
		{
			ARIA_CORE_CRITICAL("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: {0}", e.what());
		}

		const char* vertexShaderSource = vertexShaderCode.c_str();
		const char* fragmentShaderSource = fragmentShaderCode.c_str();

		// 2. compile shaders
		// vertex shader
		uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		//const GLuint vertexShader = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &vertexShaderSource);
		checkCompileErrors(vertexShader, "VERTEX");

		// fragment Shader
		uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		//const GLuint fragmentShader = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &fragmentShaderSource);

		checkCompileErrors(fragmentShader, "FRAGMENT");

		// if geometry shader is given, compile geometry shader
		GLuint geometryShader = 0;
		if (!geometryPath.empty())
		{
			const char* geometryShaderSource = geometryShaderCode.c_str();
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
			glCompileShader(geometryShader);
			//geometryShader = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &geometryShaderSource);

			checkCompileErrors(geometryShader, "GEOMETRY");
		}

		//glCreateProgramPipelines(1, &pipeline);
		//glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, vertexShader);
		//glUseProgramStages(pipeline, GL_FRAGMENT_SHADER_BIT, fragmentShader);

		//if (!geometryPath.empty())
		//{
		//	glUseProgramStages(pipeline, GL_GEOMETRY_SHADER_BIT, geometryShader);
		//}

		// shader Program
		ID = glCreateProgram();

		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);

		if (!geometryPath.empty())
			glAttachShader(ID, geometryShader);

		glLinkProgram(ID);

		checkCompileErrors(ID, "PROGRAM");

		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		if (!geometryPath.empty())
		{
			glDeleteShader(geometryShader);
		}
    }

    void loadSPV(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, const std::string& geometryShaderPath = std::string())
    {
		// 1. retrieve the vertex/fragment source code from filePath
		std::ifstream vertexShaderFile;
		std::ifstream fragmentShaderFile;
		std::ifstream geometryShaderFile;

		// ensure ifstream objects can throw exceptions:
		vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		geometryShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			// open files
			vertexShaderFile.open(vertexShaderPath, std::ios::binary);
			fragmentShaderFile.open(fragmentShaderPath, std::ios::binary);

			// if geometry shader path is present, also load a geometry shader
			if (!geometryShaderPath.empty())
			{
				geometryShaderFile.open(geometryShaderPath, std::ios::binary);
			}
		}
		catch (std::ifstream::failure& e)
		{
			ARIA_CORE_CRITICAL("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: {0}", e.what());
		}

		std::istreambuf_iterator<char> vertexShaderStreamStartIt(vertexShaderFile), vertexShaderStreamEndIt;
		std::vector<char> vertexShaderSource(vertexShaderStreamStartIt, vertexShaderStreamEndIt);
		vertexShaderFile.close();

		std::istreambuf_iterator<char> fragmentShaderStreamStartIt(fragmentShaderFile), fragmentShaderStreamEndIt;
		std::vector<char> fragmentShaderSource(fragmentShaderStreamStartIt, fragmentShaderStreamEndIt);
		fragmentShaderFile.close();

		// 2. compile shaders
		// vertex shader
		uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, vertexShaderSource.data(), static_cast<GLsizei>(vertexShaderSource.size()));
		glSpecializeShader(vertexShader, "main", 0, nullptr, nullptr);

		checkCompileErrors(vertexShader, "VERTEX");

		// fragment Shader
		uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderBinary(1, &fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V, fragmentShaderSource.data(), static_cast<GLsizei>(fragmentShaderSource.size()));
		glSpecializeShader(fragmentShader, "main", 0, nullptr, nullptr);

		checkCompileErrors(fragmentShader, "FRAGMENT");

		// if geometry shader is given, compile geometry shader
		uint32_t geometryShader = 0;
		if (!geometryShaderPath.empty())
		{
			std::istreambuf_iterator<char> geometryShaderStreamStartIt(geometryShaderFile), geometryShaderStreamEndIt;
			std::vector<char> geometryShaderSource(geometryShaderStreamStartIt, geometryShaderStreamEndIt);
			geometryShaderFile.close();

			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

			glShaderBinary(1, &geometryShader, GL_SHADER_BINARY_FORMAT_SPIR_V, geometryShaderSource.data(), static_cast<GLsizei>(geometryShaderSource.size()));
			glSpecializeShader(geometryShader, "main", 0, nullptr, nullptr);

			checkCompileErrors(geometryShader, "GEOMETRY");
		}

		// shader Program
		ID = glCreateProgram();

		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);

		if (!geometryShaderPath.empty())
			glAttachShader(ID, geometryShader);

		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		if (!geometryShaderPath.empty())
			glDeleteShader(geometryShader);
    }

    // activate the shader
    // ------------------------------------------------------------------------
    const void use() const
    { 
        glUseProgram(ID);
    }

	void bindPipeline()
	{
		glBindProgramPipeline(pipeline);
	}

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    { 
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec2(const std::string &name, float x, float y) const
    { 
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y); 
    }

	void setVec2i(const std::string& name, const glm::ivec2& value) const
	{
		glUniform2iv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    { 
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    { 
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z); 
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    { 
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]); 
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) 
    { 
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w); 
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    static void reset()
    {
        glUseProgram(0);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type)
    {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				ARIA_CORE_CRITICAL("ERROR::SHADER_COMPILATION_ERROR of type: {0}\n{1}\n{2}", type, infoLog, " -- --------------------------------------------------- -- ");
			}
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				ARIA_CORE_CRITICAL("ERROR::PROGRAM_LINKING_ERROR of type: {0}\n{1}\n{2}", type, infoLog, " -- --------------------------------------------------- -- ");
            }
        }
    }
};
#endif
