#include "Test.h"

namespace test
{
	TestMenu::TestMenu(Test*& currentTestPoint)
		:m_CurrentTest(currentTestPoint)
	{
	}

	void TestMenu::OnImGuiRender()
	{
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		for (auto& test : m_Tests)
		{
			if (ImGui::Button(test.first.c_str()))
				m_CurrentTest = test.second();
		}
	}
}