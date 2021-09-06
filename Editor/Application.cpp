// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.hpp"

#include "GraphicsCore/Instance.hpp"

#include "Engine/Asset.hpp"
#include "Engine/ShaderCompiler.hpp"

#include "Components/UI/DockSpace.hpp"
#include "Components/UI/DockerMenuBar.hpp"
#include "Components/UI/ControllerView.hpp"

#include <ImGuizmo.h>
#include <fstream>

constexpr const char DefaultRenderTargetExtent[] = "DefaultRenderTargetExtent=";

template<size_t Size>
constexpr size_t GetStringLength(const char(&string)[Size]) { return Size; }

namespace Flint
{
	Application::Application()
	{
		LoadPreviousSession();

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

		SaveCurrentSession();
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

				if (ImGui::BeginMenu("File"))
				{
					// If the user selects the load client option.
					if (ImGui::MenuItem("Load Client"))
					{
					}

					ImGui::Separator();

					ImGui::MenuItem("Close");
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("View"))
				{
					ImGui::MenuItem("Full screen");
					ImGui::MenuItem("Minimize");

					ImGui::EndMenu();
				}
			}

			HandleClientLoad();

			if (pClientLoader)
			{
				ControllerView controllerView(pClientLoader->GetInterface()->GetClassIdentifiers());
			}

			mRenderTarget.UpdateUI(delta);
			ImGui::ShowDemoWindow();

			ImGui::Begin("Frame Times");
			ImGui::Text("Frame Time: %.4f ms", frameTime);
			ImGui::Text("Average Frame Time: %.4f ms", mAverageFrameTime / mFrameCount);
			ImGui::End();
		}

		ImGui::Render();
		mRenderTarget.DrawFrame();
		mOldTimePoint = mNewTimePoint;
	}
	
	void Application::HandleClientLoad()
	{
		if (mRenderTarget.HasDragAndDrop())
		{
			const auto paths = mRenderTarget.GetDropPaths();

			for (const auto path : paths)
			{
				const auto extension = path.extension();
				if (extension.string() == ".dll")
				{
					if (pClientLoader)
						delete pClientLoader.release();

					pClientLoader = std::make_unique<ClientLoader>(path);
				}
			}
		}
	}

	void Application::LoadPreviousSession()
	{
		std::ifstream session("FlintEditorConfig.ini");

		if (!session.is_open())
			return;

		std::string line;
		while (std::getline(session, line))
		{
			if (line.find(DefaultRenderTargetExtent) != std::string::npos)
			{
				FBox2D extent2D;
				extent2D.X = std::stoi(line.substr(line.find_first_of("=") + 1, line.find_first_of(",") - line.find_first_of("=")));
				extent2D.Y = std::stoi(line.substr(line.find_first_of(",") + 1));
				mRenderTarget.SetExtent(extent2D);
			}
		}
	}

	void Application::SaveCurrentSession()
	{
		std::ofstream session("FlintEditorConfig.ini", std::ios::out | std::ios::trunc);

		if (!session.is_open())
			return;

		const auto extent = mRenderTarget.GetExtent();
		session << DefaultRenderTargetExtent << extent.X << "," << extent.Y << std::endl;
		session.close();
	}
}
