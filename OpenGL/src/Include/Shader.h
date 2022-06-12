#pragma once
#include "GLCall.h"

#include <fstream>
#include <sstream>
#include <string>

struct ShaderSource
{
	::std::string VertexSource;
	::std::string FragmentSource;
};

class Shader
{
private:  
	unsigned int m_ShaderID;
	::std::string m_FilePath;
public:
	Shader(const ::std::string& filePath);
	~Shader();

	void Bind() const;
	void Unbind()const;

private:
	ShaderSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
};