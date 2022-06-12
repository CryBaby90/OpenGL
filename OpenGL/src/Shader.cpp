#include "Shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>

Shader::Shader()
{
}

Shader::Shader(const std::string& vertexFilepath, const std::string& fragmentFilepath)
	:m_ShaderID(0), m_VertexFilepath(vertexFilepath), m_FragmentFilepath(fragmentFilepath)
{
	ShaderSource source = ParseShader(vertexFilepath, fragmentFilepath);
	m_ShaderID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_ShaderID));
}

ShaderSource Shader::ParseShader(const std::string& vertexFilepath, const std::string& fragmentFilepath)
{
	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1,
	};
	std::string line;
	std::stringstream ss[2];
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
			}
			else
				ss[(int)type] << line << "\n";
		}
	}

	return { ss[0].str(), ss[1].str() };
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

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(unsigned int shaderProgram = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	GLCall(glAttachShader(shaderProgram, vs));
	GLCall(glAttachShader(shaderProgram, fs));
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
