// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DockSpace.hpp"

namespace Flint
{
	DockSpace::DockSpace()
	{
		const ImGuiViewport* pViewPort = ImGui::GetMainViewport();

		ImGui::SetNextWindowPos(pViewPort->WorkPos);
		ImGui::SetNextWindowSize(pViewPort->WorkSize);
		ImGui::SetNextWindowViewport(pViewPort->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		mWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		mWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		if (mDockFlags & ImGuiDockNodeFlags_PassthruCentralNode)
			mWindowFlags |= ImGuiWindowFlags_NoBackground;

		ImGui::Begin("DockSpace", NULL, mWindowFlags);
		ImGui::PopStyleVar(3);

		mDockID = ImGui::GetID("EditorDockSpace");
		ImGui::DockSpace(mDockID, ImVec2(0.0f, 0.0f), mDockFlags);
	}

	DockSpace::~DockSpace()
	{
		ImGui::End();
	}
}
