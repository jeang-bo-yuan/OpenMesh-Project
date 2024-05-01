#include "ControlWindow.h"

#include <imgui.h>

#include <iostream>
#include <vector>
#include <string>

namespace CG
{
	ControlWindow::ControlWindow()
		: targetScene(nullptr), selectedState(0)
	{
		showDemoWindow = false;
	}

	auto ControlWindow::Initialize() -> bool
	{
		return true;
	}

	void ControlWindow::Display()
	{
		ImGui::Begin("Control");
		{
			ImGui::Checkbox("Demo Window", &showDemoWindow);
			ImGui::NewLine();

			const char* StateName[] = { "Rotate Camera", "Select Face"};
			selectedState = (int)targetScene->GetState();
			ImGui::Text("Left Mouse: %s", StateName[selectedState]);

			ImGui::RadioButton("Rotate Camera(1)", &selectedState, (int)State::RotateCamera);
			ImGui::RadioButton("Select Face(2)", &selectedState, (int)State::SelectFace);
			targetScene->SetState((State)selectedState);

			ImGui::NewLine();
			if (ImGui::CollapsingHeader("Tutorial")) {
				ImGui::Text("WASD - move camera   Q - move up");
				ImGui::Text("E - move down        Scroll - Zoom in/out");
			}
		}
		ImGui::End();

		// Show the big demo window or not (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
