// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DrawInstanceUI.hpp"
#include "UIPresets.hpp"

namespace Flint
{
	DrawInstanceUI::DrawInstanceUI(ComponentIdentifier& identifier, const Components::DrawInstanceGraphics& instance)
	{
		ImGui::Begin(std::string("Graphics Draw Instance - " + std::to_string(identifier.mStringHash)).c_str());

		ImGui::TextColored(TitleColor, "Wire Frames");
		for (const auto drawData : instance.mDrawData)
		{
			ImGui::Text(drawData.pWireFrame->GetName().c_str());

			ImGui::Indent();
			ImGui::Text("Vertex count: %u", drawData.pWireFrame->GetVertexCount());
			ImGui::Text("Vertex offset: %u", drawData.pWireFrame->GetVertexOffset());
			ImGui::Text("Index count: %u", drawData.pWireFrame->GetIndexCount());
			ImGui::Text("Index offset: %u", drawData.pWireFrame->GetIndexOffset());
			ImGui::Unindent();

			ImGui::Spacing();
		}

		ImGui::Separator();
		ImGui::TextColored(TitleColor, "Geometry Store");
		ImGui::Text("Vertex size: %u", instance.pGeometryStore->GetVertexSize());
		ImGui::Text("Current vertex count: %u", instance.pGeometryStore->GetVertexCount());
		ImGui::Text("Index size: %u", instance.pGeometryStore->GetIndexSize());
		ImGui::Text("Current index count: %u", instance.pGeometryStore->GetIndexCount());
	}

	DrawInstanceUI::~DrawInstanceUI()
	{
		ImGui::End();
	}
}