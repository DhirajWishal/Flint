// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ControllerView.hpp"
#include "Engine/Components/StaticModel.hpp"
#include "Engine/Components/DrawInstance.hpp"
#include "Engine/Components/MaterialPipeline.hpp"
#include "Engine/Components/Camera.hpp"
#include "Engine/Components/Uniform.hpp"

#include <imgui.h>

namespace Flint
{
	ControllerView::ControllerView(const std::vector<std::string_view>& controllers)
	{
		ImGui::Begin("Controller View");

		for (const auto identifiers : controllers)
			ImGui::Text(identifiers.data());
	}

	ControllerView::ControllerView(ClientInterface* pInterface)
	{
		ImGui::Begin("Controller View");

		//auto& globalComponentMap = pInterface->GetGlobalComponentMap();
		auto& activeControllers = pInterface->GetActiveControllers();
		for (auto& pController : activeControllers)
		{
			if (ImGui::CollapsingHeader(pController.second->GetIdentifier().data()))
			{
				ImGui::Indent();

				/*
				{
					auto pStore = pInterface->FindComponentStore<Components::StaticModel>(pController.second.get());
					if (pStore && ImGui::CollapsingHeader("Static Model"))
					{
						ImGui::Indent();
						for (const auto entry : pStore->GetArray())
							ImGui::Selectable(entry.mComponentName.data());

						ImGui::Unindent();
					}
				}

				{
					auto pStore = pInterface->FindComponentStore<Components::DrawInstanceGraphics>(pController.second.get());
					if (pStore && ImGui::CollapsingHeader("Draw Instance Graphics"))
					{
						ImGui::Indent();
						for (const auto entry : pStore->GetArray())
						{
							if (ImGui::CollapsingHeader(entry.mComponentName.data()))
							{
								ImGui::Indent();
								for (const auto drawData : entry.mDrawData)
									ImGui::Selectable(drawData.pWireFrame->GetName().c_str());
								ImGui::Unindent();
							}
						}

						ImGui::Unindent();
					}
				}

				{
					auto pStore = pInterface->FindComponentStore<Components::MaterialPipeline>(pController.second.get());
					if (pStore && ImGui::CollapsingHeader("Material Pipelines"))
					{
						ImGui::Indent();
						for (const auto entry : pStore->GetArray())
							ImGui::Selectable(entry.mComponentName.data());

						ImGui::Unindent();
					}
				}

				{
					auto pStore = pInterface->FindComponentStore<Components::Camera>(pController.second.get());
					if (pStore && ImGui::CollapsingHeader("Cameras"))
					{
						ImGui::Indent();
						for (auto& entry : pStore->GetArray())
							ImGui::Selectable(entry.mComponentName.data());

						ImGui::Unindent();
					}
				}

				{
					auto pStore = pInterface->FindComponentStore<Components::ModelViewProjectionUniform>(pController.second.get());
					if (pStore && ImGui::CollapsingHeader("Model View Projection Uniforms"))
					{
						ImGui::Indent();
						for (auto& entry : pStore->GetArray())
							ImGui::Selectable(entry.mComponentName.data());

						ImGui::Unindent();
					}
				}
				*/
				ImGui::Unindent();
			}
		}
	}

	ControllerView::~ControllerView()
	{
		ImGui::End();
	}
}