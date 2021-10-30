// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.hpp"
#include "GraphicsScene.hpp"

#include <optick.h>

namespace Flint
{
	Application::Application()
	{
		OPTICK_EVENT();

#ifdef FLINT_DEBUG
		pInstance = CreateInstance(true);

#else
		pInstance = CreateInstance(false);

#endif // FLINT_DEBUG

		pDevice = pInstance->CreateDevice(DeviceFlags::External | DeviceFlags::GraphicsCompatible | DeviceFlags::ComputeCompatible);

		// Create the empty image, image view and sampler.
		const uint8 pixelData[4] = { 255, 255, 255, 255 };
		pEmptyImage = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Graphics, FBox3D(1), PixelFormat::R8G8B8A8_SRGB, 1, 1, pixelData);
		pEmptyImageView = pEmptyImage->CreateImageView(0, 1, 0, 1, ImageUsage::Graphics);
		pEmptyImageSampler = pDevice->CreateImageSampler(ImageSamplerSpecification());
	}

	Application::~Application()
	{
	}

	void Application::PrepareNewFrame()
	{
		OPTICK_EVENT();

		NewProfilerFrame();

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
		OPTICK_EVENT();
	}

	void Application::EndFrame()
	{
		OPTICK_EVENT();

		ImGui::End();
		//ImGui::Render();
	}

	void Application::Cleanup()
	{
		pDevice->WaitIdle();
	}

	std::shared_ptr<GeometryStore> Application::CreateGeometryStore(const std::string& name, const std::vector<ShaderAttribute>& vertexAttributes, uint64 indexSize, const BufferMemoryProfile profile)
	{
		OPTICK_EVENT();

		auto pGeometryStore = pDevice->CreateGeometryStore(vertexAttributes, indexSize, profile);
		pGeometryStores[name] = pGeometryStore;

		return pGeometryStore;
	}
}