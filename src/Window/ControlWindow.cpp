#include "ControlWindow.h"

#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <iostream>
#include <vector>
#include <string>

namespace CG
{
	ControlWindow::ControlWindow()
		: targetScene(nullptr), selectedState(0), texcoordFilePath("TexCoord.txt")
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
			if (ImGui::Button("Unselect All")) targetScene->UnselectAll();
			ImGui::NewLine();

			const char* StateName[] = { "Rotate Camera", "Select Vertex", "Select Face", "Unselect Face", "Region Select", "Region Unselect"};
			selectedState = (int)targetScene->GetState();
			ImGui::Text("Left Mouse: %s", StateName[selectedState]);

			ImGui::RadioButton("Rotate Camera(1)", &selectedState, (int)State::RotateCamera);
			ImGui::SameLine(170);  ImGui::RadioButton("Select Vertex(2)", &selectedState, (int)State::SelectVertex);
			ImGui::RadioButton("Select Face(3)", &selectedState, (int)State::SelectFace);
			ImGui::SameLine(170);  ImGui::RadioButton("Unselect Face(4)", &selectedState, (int)State::UnselectFace);
			ImGui::RadioButton("Region Select(5)", &selectedState, (int)State::RegionSelectFace);
			ImGui::SameLine(170);  ImGui::RadioButton("Region Unselect(6)", &selectedState, (int)State::RegionUnselectFace);
			targetScene->SetState((State)selectedState);

			ImGui::NewLine();
			ImGui::Text("Texture Coordinate File:");
			ImGui::InputText("file", &texcoordFilePath);
			if (ImGui::Button("Load")) targetScene->LoadTexcoordFile(texcoordFilePath);
			ImGui::SameLine(170);
			if (ImGui::Button("Export")) targetScene->ExportTexcoordFile(texcoordFilePath);

			ImGui::NewLine();
			if (ImGui::CollapsingHeader("Tutorial")) {
				ImGui::Text("WASD - move camera   Q - move up");
				ImGui::Text("E - move down        Scroll - Zoom in/out");
				ImGui::Text("\nTexture Coordinate File is the file that store\nall the vertices' texture coordinate.");
			}
		}
		ImGui::End();

		// Show the big demo window or not (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
