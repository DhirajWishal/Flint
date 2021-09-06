// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DockerMenuBar.hpp"
#include "Core/DataType.hpp"

namespace Flint
{
	DockerMenuBar::DockerMenuBar(float frameTime)
	{
		ImGui::BeginMenuBar();
		//ImGui::Text("Frame time: %.6f ms", frameTime);

		if (ImGui::BeginMenu("Options"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows,
			// which we can't undo at the moment without finer window depth/z control.
			ImGui::MenuItem("Fullscreen");
			ImGui::MenuItem("Padding");
			ImGui::Separator();

			ImGui::MenuItem("Flag: NoSplit");
			ImGui::MenuItem("Flag: NoResize");
			ImGui::MenuItem("Flag: NoDockingInCentralNode");
			ImGui::MenuItem("Flag: AutoHideTabBar");
			ImGui::MenuItem("Flag: PassthruCentralNode");
			ImGui::Separator();

			ImGui::MenuItem("Close");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Save");
			ImGui::MenuItem("Save As");

			ImGui::Separator();

			ImGui::MenuItem("Close");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Full screen");
			ImGui::MenuItem("Minimize");

			ImGui::EndMenu();
		}
	}

	DockerMenuBar::~DockerMenuBar()
	{
		ImGui::EndMenuBar();
	}
}