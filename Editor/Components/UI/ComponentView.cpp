// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ComponentView.hpp"

#include <imgui.h>

namespace Flint
{
	ComponentView::ComponentView(const TComponentMap& componentMap)
	{
		ImGui::Begin("Component View");

		for (const auto components : componentMap)
		{
			if (components.first == typeid(Components::MaterialPipeline).name() && ImGui::CollapsingHeader("Material Pipelines"))
			{
				std::shared_ptr<ComponentStore<Components::MaterialPipeline>> pStore = components.second->StaticCast<ComponentStore<Components::MaterialPipeline>>().shared_from_this();

				ImGui::Indent();
				for (const auto entry : pStore->GetArray())
				{
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), entry.mParentController.data());
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), entry.mComponentName.data());
					//ImGui::Text((std::string(entry.mParentController.data()) + " - " + std::string(entry.mComponentName)).c_str());
				}

				ImGui::Unindent();
			}
		}
	}

	ComponentView::~ComponentView()
	{
		ImGui::End();
	}
}