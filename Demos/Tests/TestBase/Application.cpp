// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.hpp"
#include "GraphicsScene.hpp"

namespace Flint
{
	Application::Application()
	{
		pInstance = CreateInstance(true);
		pDevice = pInstance->CreateDevice(DeviceFlags::External | DeviceFlags::GraphicsCompatible | DeviceFlags::ComputeCompatible);
	}

	Application::~Application()
	{
		for (auto& [name, scene] : mGraphicsScenes)
			scene.Terminate();
	}

	void Application::PrepareNewFrame()
	{
		ImGui::NewFrame();

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		mWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		mWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (mDockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
			mWindowFlags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", NULL, mWindowFlags);

		ImGui::PopStyleVar(3);

		ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), mDockspaceFlags);
	}

	void Application::UpdateGraphicsScenes()
	{
		for (auto& [name, scene] : mGraphicsScenes)
			scene.Update();
	}

	void Application::DrawGraphicsScenes()
	{
		ImGui::End();
		ImGui::Render();

		for (auto& [name, scene] : mGraphicsScenes)
			scene.DrawFrame();
	}
	
	GraphicsScene* Application::CreateGraphicsScene(const std::string& name, const FBox2D extent)
	{
		mGraphicsScenes[name] = std::move(GraphicsScene(this, extent));
		return &mGraphicsScenes[name];
	}
	
	std::shared_ptr<GeometryStore> Application::CreateGeometryStore(const std::string& name, const TShaderAttributeMap& vertexAttributes, UI64 indexSize, const BufferMemoryProfile profile)
	{
		auto pGeometryStore = pDevice->CreateGeometryStore(vertexAttributes, indexSize, profile);
		pGeometryStores[name] = pGeometryStore;

		return pGeometryStore;
	}
}