// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Components/GameObject.hpp"
#include "Components/ImGui.hpp"
#include "Objects/Preview.hpp"
#include "Objects/Light.hpp"

#include <iostream>
#include <ImGuizmo.h>

int main(int argc, char** argv)
{
	try
	{
		SceneState mState{ "Flint: Point Shadow Map" };
		double average = 0.0;
		UI64 counter = 0;

		{
			Light* pLight = nullptr;
			Preview* pPreview = nullptr;

			ImGUI mImGui(glm::vec3(0.0f), &mState);
			std::vector<std::unique_ptr<GameObject>> pGameObjects;
			pGameObjects.push_back(std::make_unique<Light>(glm::vec3(1.0f), &mState));
			pGameObjects.push_back(std::make_unique<Preview>(glm::vec3(0.0f), &mState, 
				std::filesystem::path("E:\\Dynamik\\Game Repository\\InHouse\\sponza.obj"), std::vector<std::filesystem::path>{
			}));

			pLight = static_cast<Light*>(pGameObjects[0].get());
			pPreview = static_cast<Preview*>(pGameObjects[1].get());

			std::chrono::time_point<std::chrono::high_resolution_clock> oldTimePoint = std::chrono::high_resolution_clock::now();

			mState.PrepareRenderTargetsToDraw();
			while (mState.pDisplay->IsOpen())
			{
				FLINT_SETUP_PROFILER();
				std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
				UI64 delta = now.time_since_epoch().count() - oldTimePoint.time_since_epoch().count();

				mState.PrepareNewFrame();

				ImGui::NewFrame();
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::BeginFrame();

				float frameTime = delta / (1000.0f * 1000.0f);
				average += frameTime;
				counter++;

				ImGui::Text("Frame Time: %f ms", frameTime);
				ImGui::Text("Average Frame Time: %f ms", average / counter);

				ImGuiIO& io = ImGui::GetIO();

				{
					io.DeltaTime = frameTime / 1000.0f;

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
					mState.UpdateCamera(delta);

				//for (auto& pGameObject : pGameObjects)
				//	pGameObject->OnUpdate(delta);

				pLight->OnUpdate(delta);

				pPreview->OnUpdate(delta);
				pPreview->SetLightPosition(pLight->GetPosition());
				//pLight->SetPosition(pPreview->GetLightPosition());

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

/*
std::shared_ptr<Flint::SwapChain> pSwapChain = pDevice->CreateSwapChain(pDisplay, Flint::SwapChainMode::MAIL_BOX, bufferCount);
std::shared_ptr<Flint::Image> pColorBuffer = pDevice->CreateColorBuffer(bufferCount, pSwapChain->GetPixelFormat());
std::shared_ptr<Flint::Image> pDepthBuffer = pDevice->CreateDepthBuffer(bufferCount, pDevice->GetBestDepthFormat());
*/
