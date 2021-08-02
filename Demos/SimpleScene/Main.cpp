// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Components/GameObject.hpp"
#include "Components/Objects/VikingRoom.hpp"
#include "Components/Objects/SkyBox.hpp"

#include <iostream>

int main()
{
	try
	{
		SceneState mState{};

		{
			SkyBox skyBox{ &mState };
			std::unique_ptr<VikingRoom> pVikingRoom = nullptr;

			mState.PrepareRenderTargetsToDraw();
			while (mState.pDisplay->IsOpen())
			{
				FLINT_SETUP_PROFILER();
				mState.PrepareNewFrame();

				mState.UpdateCamera();
				skyBox.OnUpdate();

				if (mState.pDisplay->GetKeyEvent(Flint::KeyCode::KEY_L).IsPressed())
				{
					if (!pVikingRoom)
						pVikingRoom = std::make_unique<VikingRoom>(&mState);
					else if (pVikingRoom)
					{
						auto pPointer = pVikingRoom.release();
						delete pPointer;
					}

					using namespace std::literals;
					std::this_thread::sleep_for(100ms);
				}

				if (pVikingRoom)
					pVikingRoom->OnUpdate();

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