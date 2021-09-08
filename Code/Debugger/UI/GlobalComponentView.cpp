// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GlobalComponentView.hpp"
#include "Engine/Controller.hpp"
#include "Engine/ComponentSystem.hpp"
#include "Engine/Components/StaticModel.hpp"
#include "Engine/Components/DrawInstance.hpp"
#include "Engine/Components/MaterialPipeline.hpp"
#include "Engine/Components/Camera.hpp"
#include "Engine/Components/Uniform.hpp"

#include <imgui.h>

namespace Flint
{
	GlobalComponentView::GlobalComponentView(TGlobalComponentMap& componentMap)
	{
		ImGui::Begin("Global Component View");

		for (const auto components : componentMap)
		{
			if (components.first == typeid(Components::MaterialPipeline).name() && ImGui::CollapsingHeader("Material Pipelines"))
			{
				std::shared_ptr<ComponentStore<Components::MaterialPipeline>> pStore = components.second->StaticCast<ComponentStore<Components::MaterialPipeline>>().shared_from_this();

				ImGui::Indent();
				for (const auto entry : pStore->GetArray())
					ImGui::Text(entry.mComponentName.data());

				ImGui::Unindent();
			}

			if (components.first == typeid(Components::StaticModel).name() && ImGui::CollapsingHeader("Static Models"))
			{
				std::shared_ptr<ComponentStore<Components::StaticModel>> pStore = components.second->StaticCast<ComponentStore<Components::StaticModel>>().shared_from_this();

				ImGui::Indent();
				for (const auto entry : pStore->GetArray())
					ImGui::Text(entry.mComponentName.data());

				ImGui::Unindent();
			}

			if (components.first == typeid(Components::DrawInstanceGraphics).name() && ImGui::CollapsingHeader("Draw Instances"))
			{
				std::shared_ptr<ComponentStore<Components::DrawInstanceGraphics>> pStore = components.second->StaticCast<ComponentStore<Components::DrawInstanceGraphics>>().shared_from_this();

				ImGui::Indent();
				for (const auto entry : pStore->GetArray())
				{
					if (ImGui::CollapsingHeader(entry.mComponentName.data()))
					{
						ImGui::Indent();
						for (const auto drawData : entry.mDrawData)
							ImGui::Text(drawData.pWireFrame->GetName().c_str());

						ImGui::Unindent();
					}
				}

				ImGui::Unindent();
			}

			if (components.first == typeid(Components::Camera).name() && ImGui::CollapsingHeader("Cameras"))
			{
				std::shared_ptr<ComponentStore<Components::Camera>> pStore = components.second->StaticCast<ComponentStore<Components::Camera>>().shared_from_this();

				ImGui::Indent();
				for (auto& entry : pStore->GetArray())
					ImGui::Text(entry.mComponentName.data());

				ImGui::Unindent();
			}

			if (components.first == typeid(Components::ModelViewProjectionUniform).name() && ImGui::CollapsingHeader("Model View Projection"))
			{
				std::shared_ptr<ComponentStore<Components::ModelViewProjectionUniform>> pStore = components.second->StaticCast<ComponentStore<Components::ModelViewProjectionUniform>>().shared_from_this();

				ImGui::Indent();
				for (auto& entry : pStore->GetArray())
					ImGui::Text(entry.mComponentName.data());

				ImGui::Unindent();
			}
		}
	}

	GlobalComponentView::~GlobalComponentView()
	{
		ImGui::End();
	}
}