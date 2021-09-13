// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "StaticModelUI.hpp"
#include "UIPresets.hpp"

namespace Flint
{
	StaticModelUI::StaticModelUI(ComponentIdentifier& identifier, const Components::StaticModel& model)
	{
		ImGui::Begin(std::string("Static Model - " + std::to_string(identifier.mStringHash)).c_str());

		ImGui::TextColored(TitleColor, "Wire Frames");
		for (const auto wireFrame : model.mWireFrames)
		{
			ImGui::Text(wireFrame.GetName().c_str());

			ImGui::Indent();
			ImGui::Text("Vertex count: %u", wireFrame.GetVertexCount());
			ImGui::Text("Vertex offset: %u", wireFrame.GetVertexOffset());
			ImGui::Text("Index count: %u", wireFrame.GetIndexCount());
			ImGui::Text("Index offset: %u", wireFrame.GetIndexOffset());
			ImGui::Unindent();

			ImGui::Spacing();
		}

		ImGui::Separator();
		ImGui::TextColored(TitleColor, "Geometry Store");
		ImGui::Text("Vertex size: %u", model.pGeometryStore->GetVertexSize());
		ImGui::Text("Current vertex count: %u", model.pGeometryStore->GetVertexCount());
		ImGui::Text("Index size: %u", model.pGeometryStore->GetIndexSize());
		ImGui::Text("Current index count: %u", model.pGeometryStore->GetIndexCount());
	}

	StaticModelUI::~StaticModelUI()
	{
		ImGui::End();
	}
}
