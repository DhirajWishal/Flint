// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ComponentView.hpp"
#include "Engine/Controller.hpp"

#include "Engine/Components/StaticModel.hpp"
#include "Engine/Components/DrawInstance.hpp"
#include "Engine/Components/MaterialPipeline.hpp"
#include "Engine/Components/Camera.hpp"
#include "Engine/Components/Uniform.hpp"

#include "Components/MaterialPipelineUI.hpp"
#include "Components/StaticModelUI.hpp"

#include <imgui.h>

namespace Flint
{
	static std::pair<ComponentIdentifier, const Components::MaterialPipeline*> selectedPipeline;
	static std::pair<ComponentIdentifier, const Components::StaticModel*> selectedModel;

	ComponentView::ComponentView(const TComponentStore& componentMap)
	{
		ImGui::Begin("Component View");

		for (auto& controllers : componentMap)
		{
			if (controllers.first == typeid(Components::MaterialPipeline).name() && ImGui::CollapsingHeader("Material Pipelines"))
			{
				std::shared_ptr<ComponentStore<Components::MaterialPipeline>> pStore = std::static_pointer_cast<ComponentStore<Components::MaterialPipeline>>(controllers.second);

				ImGui::Indent();
				for (auto& entry : *pStore)
				{
					if (ImGui::Selectable(std::string("Material Identifier: " + std::to_string(entry.first.mStringHash)).c_str()))
						selectedPipeline = { entry.first , &entry.second };
				}

				ImGui::Unindent();
			}

			if (controllers.first == typeid(Components::StaticModel).name() && ImGui::CollapsingHeader("Static Models"))
			{
				std::shared_ptr<ComponentStore<Components::StaticModel>> pStore = std::static_pointer_cast<ComponentStore<Components::StaticModel>>(controllers.second);

				ImGui::Indent();
				for (auto& entry : *pStore)
				{
					if (ImGui::Selectable(std::string("Static Model Identifier: " + std::to_string(entry.first.mStringHash)).c_str()))
						selectedModel = { entry.first , &entry.second };
				}

				ImGui::Unindent();
			}

			if (controllers.first == typeid(Components::DrawInstanceGraphics).name() && ImGui::CollapsingHeader("Draw Instances"))
			{
				std::shared_ptr<ComponentStore<Components::DrawInstanceGraphics>> pStore = std::static_pointer_cast<ComponentStore<Components::DrawInstanceGraphics>>(controllers.second);

				ImGui::Indent();
				for (auto& entry : *pStore)
				{
					if (ImGui::Selectable(std::string("Graphics Draw Instance Identifier: " + std::to_string(entry.first.mStringHash)).c_str()))
						selectedModel = { entry.first , &entry.second };
				}
				ImGui::Unindent();
			}

			/*
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
		*/
		}
	}

	ComponentView::~ComponentView()
	{
		ImGui::End();

		if (selectedPipeline.second)
			MaterialPipelineUI ui(selectedPipeline.first, *selectedPipeline.second);

		if (selectedModel.second)
			StaticModelUI ui(selectedModel.first, *selectedModel.second);
	}
}