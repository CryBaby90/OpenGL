#pragma once
#include "GLCall.h"

#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <unordered_map>

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
	// caching for uniforms
	mutable ::std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader();
	Shader(const std::string& vertexFilepath, const std::string& fragmentFilepath);
	~Shader();

	void Bind() const;
	void Unbind()const;

	// Set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1iv(const std::string& name, int count, int* value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniforms2f(const std::string& name, const glm::vec2& value);
	void SetUniforms3f(const std::string& name, const glm::vec3& value);
	void SetUniforms4f(const std::string& name, const glm::vec4& value);

	void SetUniformsMat3f(const std::string& name, const glm::mat3& matrix);
	void SetUniformsMat4f(const std::string& name, const glm::mat4& matrix);

private:
	ShaderSource ParseShader(const std::string& vertexFilepath, const std::string& fragmentFilepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	int GetUniformLocation(const std::string& name) const;
};