// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Components/GameObject.hpp"
#include "Components/ImGui.hpp"
#include "Objects/Preview.hpp"

#include <iostream>

int main(int argc, char** argv)
{
	try
	{
		SceneState mState{ "Flint: ShadowMap" };

		float floatArray[100] = {};
		float minFrameTime = std::numeric_limits<float>::max();
		float maxFrameTime = 0.0f;

		{
			ImGUI mImGui(glm::vec3(0.0f), &mState);
			std::vector<std::unique_ptr<GameObject>> pGameObjects;
			pGameObjects.push_back(std::make_unique<Preview>(glm::vec3(0.0f), &mState, std::filesystem::path("E:\\Dynamik\\Game Repository\\InHouse\\DemoScene.obj"), std::vector<std::filesystem::path>{}));

			std::chrono::time_point<std::chrono::high_resolution_clock> oldTimePoint = std::chrono::high_resolution_clock::now();

			mState.PrepareRenderTargetsToDraw();
			while (mState.pDisplay->IsOpen())
			{
				FLINT_SETUP_PROFILER();
				std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
				UI64 delta = now.time_since_epoch().count() - oldTimePoint.time_since_epoch().count();

				mState.PrepareNewFrame();

				ImGui::NewFrame();

				std::rotate(floatArray, floatArray + 1, floatArray + 100);
				float frameTime = delta / (1000.0f * 1000.0f * 1000.0f);
				floatArray[99] = frameTime;

				if (frameTime < minFrameTime)
					minFrameTime = frameTime;

				if (frameTime > maxFrameTime)
					maxFrameTime = frameTime;

				ImGui::PlotLines("Frame Times", &floatArray[0], 50, 0, "", minFrameTime, maxFrameTime, ImVec2(0, 80));

				ImGuiIO& io = ImGui::GetIO();

				{
					io.DeltaTime = delta / (1000.0f * 1000.0f * 1000.0f);

					auto extent = mState.pDisplay->GetExtent();
					if (!extent.IsZero())
						io.DisplaySize = ImVec2(static_cast<float>(extent.mWidth), static_cast<float>(extent.mHeight));

					auto position = mState.pDisplay->GetMousePosition();
					io.MousePos = ImVec2(position.X, position.Y);

					if (mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::LEFT).IsPressed() || mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::LEFT).IsOnRepeat())
						io.MouseDown[0] = true;
					else if (mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::LEFT).IsReleased())
						io.MouseDown[0] = false;

					if (mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::RIGHT).IsPressed() || mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::RIGHT).IsOnRepeat())
						io.MouseDown[1] = true;
					else if (mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::RIGHT).IsReleased())
						io.MouseDown[1] = false;
				}


				if (!io.WantCaptureMouse)
				{
					mState.UpdateCamera(delta);

					const auto paths = mState.pDisplay->GetDragAndDropValues();

					if (paths.size())
					{
						for (auto path : paths)
							std::cout << path << std::endl;
					}

					for (auto& pGameObject : pGameObjects)
						pGameObject->OnUpdate(delta);
				}

				ImGui::Render();
				mImGui.OnUpdate(0);

				oldTimePoint = now;
				mState.SubmitFrames();
				mState.pDisplay->Update();
			}

			int finish = true;
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}