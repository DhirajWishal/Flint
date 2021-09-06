// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.hpp"

#include "GraphicsCore/Instance.hpp"

#include "Engine/Asset.hpp"
#include "Engine/ShaderCompiler.hpp"

#include "Components/UI/DockSpace.hpp"
#include "Components/UI/DockerMenuBar.hpp"

#include <ImGuizmo.h>

namespace Flint
{
	Application::Application()
	{
		pInstance = CreateInstance(true);
		pDevice = pInstance->CreateDevice(DeviceFlags::External | DeviceFlags::ComputeCompatible | DeviceFlags::GraphicsCompatible);

		// Initialize the main render target.
		mRenderTarget.Initialize(pDevice, pInstance);

		// Main execution loop.
		mOldTimePoint = std::chrono::steady_clock::now();
		while (mRenderTarget.IsDisplayOpen() && bShouldRun)
			Execute();
	}

	Application::~Application()
	{
		mRenderTarget.Terminate();

		pDevice->Terminate();
		pInstance->Terminate();
	}

	void Application::Execute()
	{
		mNewTimePoint = std::chrono::steady_clock::now();
		UI64 delta = mNewTimePoint.time_since_epoch().count() - mOldTimePoint.time_since_epoch().count();

		mRenderTarget.PollEvents(delta);

		ImGui::NewFrame();
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::BeginFrame();

		{
			DockSpace dockSpace;

			float frameTime = delta / (1000.0f * 1000.0f);
			mAverageFrameTime += frameTime;
			mFrameCount++;

			{
				DockerMenuBar menuBar(frameTime);
			}

			mRenderTarget.UpdateUI(delta);
			ImGui::ShowDemoWindow();

			ImGui::Begin("Frame Times");
			ImGui::Text("Frame Time: %.4f ms", frameTime);
			ImGui::Text("Average Frame Time: %.4f ms", mAverageFrameTime / mFrameCount);
			ImGui::End();
		}

		mRenderTarget.DrawFrame();
		mOldTimePoint = mNewTimePoint;
	}
}
