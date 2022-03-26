// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DataType.hpp"
#include <deque>

namespace Flint
{
	enum class ReactorType : uint8 {
		Vulkan
	};

	enum class Status : uint8 {
		Pending,
		Executing,
		Error,
		Successful
	};

	/**
	 * Command class.
	 * This class is the base class for all the commands used by the reactor controller.
	 */
	class Command
	{
	public:
		Command(Status* pStatus = nullptr) : pStatus(pStatus) {}

	private:
		Status* pStatus = nullptr;
	};

	/**
	 * Reactor controller class.
	 * This class is used to manage and send commands to a single reactor.
	 */
	class ReactorController
	{
	public:
		/**
		 * Construct a reactor controller using the reactor type.
		 * This will create the appropriate reactor and manages its life time.
		 *
		 * @param type The reactor type.
		 */
		ReactorController(ReactorType type);

	private:
		std::jthread mReactorThread;
		std::deque<std::unique_ptr<Command>> mCommandQueue;
		std::mutex mResourceMutex;
	};
}