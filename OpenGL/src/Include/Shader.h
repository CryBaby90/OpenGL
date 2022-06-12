#pragma once
#include "GLCall.h"

#include <string>
#include <iostream>

struct ShaderSource
{
	::std::string VertexSource;
	::std::string FragmentSource;
};

class Shader
{
private:  
	unsigned int m_ShaderID;
	::std::string m_VertexFilepath;
	::std::string m_FragmentFilepath;
public:
	Shader(const std::string& vertexFilepath, const std::string& fragmentFilepath);
	~Shader();

	void Bind() const;
	void Unbind()const;

private:
	ShaderSource ParseShader(const std::string& vertexFilepath, const std::string& fragmentFilepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
};