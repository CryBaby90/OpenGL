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
	::std::string GeometrySource;
};

class Shader
{
private:  
	unsigned int m_ShaderID;
	::std::string m_VertexFilepath;
	::std::string m_FragmentFilepath;
	::std::string m_geometryFilepath;
	::std::string m_NullPath = "null";
	// caching for uniforms
	mutable ::std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& vertexFilepath, const std::string& fragmentFilepath);
	Shader(const std::string& vertexFilepath, const std::string& fragmentFilepath, const std::string& geometryFilepath);
	~Shader();

	void Bind() const;
	void Unbind()const;

	unsigned int GetShaderID() const;

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
	ShaderSource ParseShader(const std::string& vertexFilepath, const std::string& fragmentFilepath, const std::string& geometryFilepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader);
	int GetUniformLocation(const std::string& name) const;
};