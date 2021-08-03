// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Components/GameObject.hpp"
#include "Components/Objects/VikingRoom.hpp"
#include "Components/Objects/SkyBox.hpp"
#include "Components/Objects/TreeScene.hpp"

#include <iostream>

int main()
{
	try
	{
		SceneState mState{};

		{
			SkyBox skyBox{ glm::vec3(0.0f), &mState };
			//VikingRoom vikingRoom{ glm::vec3(2.0f), &mState };
			//TreeScene treeScene{ glm::vec3(0.0f), &mState };
			std::unique_ptr<TreeScene> pTreeScene = nullptr;

			mState.PrepareRenderTargetsToDraw();
			while (mState.pDisplay->IsOpen())
			{
				FLINT_SETUP_PROFILER();
				mState.PrepareNewFrame();

				if (mState.pDisplay->GetKeyEvent(Flint::KeyCode::KEY_L).IsPressed())
				{
					if (!pTreeScene)
						pTreeScene = std::make_unique<TreeScene>(glm::vec3(0.0f), &mState);
				}

				if (mState.pDisplay->GetKeyEvent(Flint::KeyCode::KEY_U).IsPressed())
				{
					if (pTreeScene)
					{
						auto pTree = pTreeScene.release();
						delete pTree;
					}
				}

				mState.UpdateCamera();
				skyBox.OnUpdate();
				//vikingRoom.OnUpdate();
				//treeScene.OnUpdate();

				if (pTreeScene)
					pTreeScene->OnUpdate();

				mState.SubmitFrames();
				mState.pDisplay->Update();
			}
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}