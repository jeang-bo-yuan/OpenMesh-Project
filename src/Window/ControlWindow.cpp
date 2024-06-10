#include "ControlWindow.h"

#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <iostream>
#include <vector>
#include <string>

namespace CG
{
	ControlWindow::ControlWindow()
		: targetScene(nullptr), selectedState(0), texcoordFilePath("TexCoord.txt"), textureLoaded{false}
	{
		showDemoWindow = false;

		for (int i = 0; i < TextureManager::MAX_IMG; ++i) {
			textureFileArray[i] = std::to_string(i).append(".png");
		}
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
			ImGui::Checkbox("Show Mesh's Line", &showMeshLine); targetScene->ShowMeshLine(showMeshLine);
			if (ImGui::Button("Unselect All")) targetScene->UnselectAll();
			ImGui::NewLine();

			const char* StateName[] = { "Rotate Camera", "Select Vertex", "Select Face", "Unselect Face", "Region Select", "Region Unselect"};
			selectedState = (int)targetScene->GetState();
			ImGui::Text("Left Mouse: %s", StateName[selectedState]);

			ImGui::RadioButton("Rotate Camera(1)", &selectedState, (int)State::RotateCamera);
			ImGui::SameLine(200);  ImGui::RadioButton("Select Vertex(2)", &selectedState, (int)State::SelectVertex);
			ImGui::RadioButton("Select Face(3)", &selectedState, (int)State::SelectFace);
			ImGui::SameLine(200);  ImGui::RadioButton("Unselect Face(4)", &selectedState, (int)State::UnselectFace);
			ImGui::RadioButton("Region Select(5)", &selectedState, (int)State::RegionSelectFace);
			ImGui::SameLine(200);  ImGui::RadioButton("Region Unselect(6)", &selectedState, (int)State::RegionUnselectFace);
			targetScene->SetState((State)selectedState);

			ImGui::NewLine();
			if (ImGui::CollapsingHeader("Tutorial")) {
				ImGui::Text("WASD - move camera   Q - move up");
				ImGui::Text("E - move down        Scroll - Zoom in/out");
			}
		}
		ImGui::End();

		ImGui::Begin("Texture Array");
		{
			ImGui::Text("You can load %d textures at most.", TextureManager::MAX_IMG);
			ImGui::BulletText("Each texture should have size: %d * %d", TextureManager::IMG_SIZE, TextureManager::IMG_SIZE);

			for (int i = 0; i < TextureManager::MAX_IMG; ++i) {
				ImGui::PushID(i);
				
				if (textureLoaded[i])
					ImGui::TextColored(ImVec4(0, 1, 0, 1), "%d", i);
				else
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "%d", i);
				
				ImGui::SameLine();
				ImGui::InputText("##file", textureFileArray + i);

				ImGui::SameLine();
				if (ImGui::Button("Load")) textureLoaded[i] = targetScene->LoadImage(textureFileArray[i], i);

				ImGui::PopID();
			}
		}
		ImGui::End();

		ImGui::Begin("Texture Parameterization");
		{
			ImGui::Text("Texture Coordinate File:");
			ImGui::InputText("##file", &texcoordFilePath);
			if (ImGui::Button("Load")) targetScene->LoadTexcoordFile(texcoordFilePath);
			ImGui::SameLine(170);
			if (ImGui::Button("Export")) targetScene->ExportTexcoordFile(texcoordFilePath);

			ImGui::NewLine();
			ImGui::BulletText("-1 means deleting texture coordinate.");
			ImGui::InputInt("Texture Layer", &textureLayer);
			textureLayer = glm::clamp(textureLayer, -1, TextureManager::MAX_IMG - 1);

			if (ImGui::Button("Generate")) targetScene->GenerateTexCoord(textureLayer);
		}
		ImGui::End();

		// Show the big demo window or not (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);
	}
}
