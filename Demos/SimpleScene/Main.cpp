// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Components/GameObject.hpp"
#include "Components/Objects/VikingRoom.hpp"
#include "Components/Objects/SkyBox.hpp"

#include <iostream>

int main()
{
	//try
	//{
		SceneState mState{};

		{
			SkyBox skyBox{ &mState };
			VikingRoom vikingRoom{ &mState };

			mState.PrepareRenderTargetsToDraw();
			while (mState.pDisplay->IsOpen())
			{
				FLINT_SETUP_PROFILER();
				mState.PrepareNewFrame();

				mState.UpdateCamera();
				skyBox.OnUpdate();
				vikingRoom.OnUpdate();

				mState.SubmitFrames();
				mState.pDisplay->Update();
			}
		}
	//}
	//catch (std::exception& e)
	//{
	//	std::cout << e.what() << std::endl;
	//}

	return 0;
}