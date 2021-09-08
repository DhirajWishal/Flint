// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DockerMenuBar.hpp"
#include "Core/DataType.hpp"

namespace Flint
{
	DockerMenuBar::DockerMenuBar(float frameTime)
	{
		ImGui::BeginMenuBar();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load Client")) {}

			ImGui::Separator();

			ImGui::MenuItem("Close");
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			ImGuiIO& io = ImGui::GetIO();
			ImFont* pCurrentFont = ImGui::GetFont();
			if (ImGui::BeginCombo("Select Font", pCurrentFont->GetDebugName()))
			{
				for (UI32 i = 0; i < static_cast<UI32>(io.Fonts->Fonts.Size); i++)
				{
					ImFont* pFont = io.Fonts->Fonts[i];

					ImGui::PushID(pFont);
					if (ImGui::Selectable(pFont->GetDebugName(), pFont == pCurrentFont))
						io.FontDefault = pFont;

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			ImGui::EndMenu();
		}
	}

	DockerMenuBar::~DockerMenuBar()
	{
		ImGui::EndMenuBar();
	}
}