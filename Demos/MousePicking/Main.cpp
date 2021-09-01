// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Components/GameObject.hpp"
#include "Components/ImGui.hpp"
#include "Objects/VikingRoom.hpp"
#include "Objects/SkyBox.hpp"

#include <iostream>
#include <ImGuizmo.h>

int main(int argc, char** argv)
{
	try
	{
		SceneState mState{ "Flint: Mouse Picking" };

		{
			ImGUI mImGui(glm::vec3(0.0f), &mState);
			std::vector<std::unique_ptr<GameObject>> pGameObjects;
			pGameObjects.push_back(std::make_unique<SkyBox>(glm::vec3(0.0f), &mState));
			pGameObjects.push_back(std::make_unique<VikingRoom>(glm::vec3(2.0f), &mState));

			std::chrono::time_point<std::chrono::high_resolution_clock> oldTimePoint = std::chrono::high_resolution_clock::now();

			mState.PrepareRenderTargetsToDraw();
			while (mState.pDisplay->IsOpen())
			{
				FLINT_SETUP_PROFILER();
				std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
				UI64 delta = now.time_since_epoch().count() - oldTimePoint.time_since_epoch().count();

				mState.PrepareNewFrame();

				ImGui::NewFrame();
				ImGuizmo::BeginFrame();
				ImGuizmo::SetOrthographic(false);

				float frameTime = delta / (1000.0f * 1000.0f);
				ImGui::Text("Frame Time: %f ms", frameTime);

				ImGuiIO& io = ImGui::GetIO();

				{
					io.DeltaTime = delta / (1000.0f * 1000.0f * 1000.0f);

					auto extent = mState.pDisplay->GetExtent();
					if (!extent.IsZero())
						io.DisplaySize = ImVec2(static_cast<float>(extent.mWidth), static_cast<float>(extent.mHeight));

					auto position = mState.pDisplay->GetMousePosition();
					io.MousePos = ImVec2(position.X, position.Y);

					if (mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::Left).IsPressed() || mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::Left).IsOnRepeat())
						io.MouseDown[0] = true;
					else if (mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::Left).IsReleased())
						io.MouseDown[0] = false;

					if (mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::Right).IsPressed() || mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::Right).IsOnRepeat())
						io.MouseDown[1] = true;
					else if (mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::Right).IsReleased())
						io.MouseDown[1] = false;
				}

				if (!io.WantCaptureMouse)
				{
					mState.UpdateCamera(delta);
				}

				for (auto& pGameObject : pGameObjects)
					pGameObject->OnUpdate(delta);

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