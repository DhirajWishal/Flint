// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/Reactors/ReactorController.hpp"
#include "Graphics/Reactors/VulkanReactor.hpp"

namespace Flint
{
	namespace Reactors
	{
		template<class Type>
		void ReactorFunction(std::deque<std::unique_ptr<Command>>& commandQueue, std::mutex const& resourceMutex)
		{
			//Type reactor{ commandQueue, resourceMutex };
		}
	}

	namespace Commands
	{
		class Initialize : public Command {};
	}

	ReactorController::ReactorController(ReactorType type)
	{
		//if (type == ReactorType::Vulkan)
		//	mReactorThread = std::jthread(Reactors::ReactorFunction<VulkanReactor>, mCommandQueue, mResourceMutex);
	}
}