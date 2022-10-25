#include "Shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& vertexFilepath, const std::string& fragmentFilepath)
	:m_ShaderID(0), m_VertexFilepath(vertexFilepath), m_FragmentFilepath(fragmentFilepath)
{
	ShaderSource source = ParseShader(vertexFilepath, fragmentFilepath, m_NullPath);
	m_ShaderID = CreateShader(source.VertexSource, source.FragmentSource, source.GeometrySource);
}

Shader::Shader(const std::string& vertexFilepath, const std::string& fragmentFilepath, const std::string& geometryFilepath)
	:m_ShaderID(0), m_VertexFilepath(vertexFilepath), m_FragmentFilepath(fragmentFilepath), m_geometryFilepath(geometryFilepath)
{
	ShaderSource source = ParseShader(vertexFilepath, fragmentFilepath, geometryFilepath);
	m_ShaderID = CreateShader(source.VertexSource, source.FragmentSource, source.GeometrySource);
}


Shader::~Shader()
{
	GLCall(glDeleteProgram(m_ShaderID));
}

ShaderSource Shader::ParseShader(const std::string& vertexFilepath, const std::string& fragmentFilepath, const std::string& geometryFilepath)
{
	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1,
		GEOMETRY = 2,
	};
	std::string line;
	std::stringstream ss[3];
	ShaderType type = ShaderType::NONE;

	{
		::std::ifstream stream(vertexFilepath);
		while (getline(stream, line))
		{
			if (line.find("#shader") != ::std::string::npos)
			{
				if (line.find("vertex") != ::std::string::npos)
					type = ShaderType::VERTEX;
				else if (line.find("fragment") != ::std::string::npos)
					type = ShaderType::FRAGMENT;
				else if (line.find("geometry") != ::std::string::npos)
					type = ShaderType::GEOMETRY;
			}
			else
				ss[(int)type] << line << "\n";
		}
	}
	{
		::std::ifstream stream(fragmentFilepath);
		while (getline(stream, line))
		{
			if (line.find("#shader") != ::std::string::npos)
			{
				if (line.find("vertex") != ::std::string::npos)
					type = ShaderType::VERTEX;
				else if (line.find("fragment") != ::std::string::npos)
					type = ShaderType::FRAGMENT;
				else if (line.find("geometry") != ::std::string::npos)
					type = ShaderType::GEOMETRY;
			}
			else
				ss[(int)type] << line << "\n";
		}
	}
	{
		if (geometryFilepath != m_NullPath)
		{
			::std::ifstream stream(geometryFilepath);
			while (getline(stream, line))
			{
				if (line.find("#shader") != ::std::string::npos)
				{
					if (line.find("vertex") != ::std::string::npos)
						type = ShaderType::VERTEX;
					else if (line.find("fragment") != ::std::string::npos)
						type = ShaderType::FRAGMENT;
					else if (line.find("geometry") != ::std::string::npos)
						type = ShaderType::GEOMETRY;
				}
				else
					ss[(int)type] << line << "\n";
			}
		}
		else
		{
			return { ss[0].str(), ss[1].str(), m_NullPath };
		}
	}
	return { ss[0].str(), ss[1].str(), ss[2].str()};
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	GLCall(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, NULL));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));//检测shader是否编译成功
	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* msg = (char*)malloc(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, msg));
		std::cout << "Failed to compile shader!" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << msg << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader)
{
	GLCall(unsigned int shaderProgram = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	GLCall(glAttachShader(shaderProgram, vs));
	GLCall(glAttachShader(shaderProgram, fs));
	if (geometryShader != m_NullPath)
	{
		unsigned int gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
		GLCall(glAttachShader(shaderProgram, gs));
	}
	GLCall(glLinkProgram(shaderProgram));

	int result;
	GLCall(glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result));//检测链接着色器是否成功
	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length));
		char* msg = (char*)malloc(length * sizeof(char));
		GLCall(glGetProgramInfoLog(shaderProgram, length, &length, msg));
		std::cout << "Failed to Link shader!" << std::endl;
		std::cout << msg << std::endl;
		GLCall(glDeleteProgram(shaderProgram));
		return 0;
	}

	GLCall(glValidateProgram(shaderProgram)); //用于调试
	GLCall(glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &result));//检查是否成功
	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &length));
		char* msg = (char*)malloc(length * sizeof(char));
		GLCall(glGetProgramInfoLog(shaderProgram, length, &length, msg));
		std::cout << "Failed to Use shaderProgram!" << std::endl;
		std::cout << msg << std::endl;
		GLCall(glDeleteProgram(shaderProgram));
		return 0;
	}
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));
	return shaderProgram;
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_ShaderID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

unsigned int Shader::GetShaderID() const
{
	return m_ShaderID;
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform1iv(const std::string& name, int count, int* value)
{
	GLCall(glUniform1iv(GetUniformLocation(name), count, value));
}

void Shader::SetUniform1f(const std::string& name, float value)
{
	GLCall(glUniform1f(GetUniformLocation(name), value));
}

void Shader::SetUniforms2f(const std::string& name, const glm::vec2& value)
{
	GLCall(glUniform2f(GetUniformLocation(name), value.x, value.y));
}

void Shader::SetUniforms3f(const std::string& name, const glm::vec3& value)
{
	GLCall(glUniform3f(GetUniformLocation(name), value.x, value.y, value.z));
}

void Shader::SetUniforms4f(const std::string& name, const glm::vec4& value)
{
	GLCall(glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w));
}

void Shader::SetUniformsMat3f(const std::string& name, const glm::mat3& matrix)
{
	GLCall(glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix)));
}

void Shader::SetUniformsMat4f(const std::string& name, const glm::mat4& matrix)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix)));
}

int Shader::GetUniformLocation(const std::string& name) const
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLCall(int localtion = glGetUniformLocation(m_ShaderID, name.c_str()));
	if (localtion == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;

	m_UniformLocationCache[name] = localtion;
	return localtion;
}
