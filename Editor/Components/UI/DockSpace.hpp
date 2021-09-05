// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <imgui.h>

/**
 * ImGui dock space wrapper.
 */
class DockSpace
{
public:
	DockSpace();
	~DockSpace();

private:
	ImGuiID mDockID = 0;
	ImGuiDockNodeFlags mDockFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;
	ImGuiWindowFlags mWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
};