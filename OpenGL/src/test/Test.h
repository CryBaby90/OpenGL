#pragma once
#include "GLCall.h"

#include <imgui/imgui.h>
#include <iostream>
#include <vector>
#include <functional>

namespace test
{
	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}

		virtual void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset){}
		virtual void OnScroll(GLfloat xoffset, GLfloat yoffset){}
		virtual void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}	
	};

	class TestMenu : public Test
	{
	public:
		TestMenu(Test*& currentTestPoint);

		void OnImGuiRender() override;

		template<typename T>
		void RegisterTest(const std::string& name)
		{
			std::cout << "RegisterTest: " << name << std::endl;
			m_Tests.push_back(std::make_pair(name, []() {return new T(); }));
		}
	private:
		Test*& m_CurrentTest;
		std::vector<std::pair<std::string, std::function<Test* ()>>> m_Tests;
	};
}

