// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <thread>
#include <list>
#include <memory>
#include <functional>
#include "CommandBase.hpp"

namespace Flint
{
	/**
	 * Reactor class.
	 * This object is the base class for all the reactors in the engine.
	 * It contains and manages the command structure which contains the commands in the order of submission.
	 */
	class Reactor
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param function The worker function of the reactor.
		 * @param args The additional variadic arguments used by the worker.
		 */
		template<class... Args>
		explicit Reactor(std::function<void(std::list<std::unique_ptr<CommandBase>>&, std::mutex&, Args...)> function, Args&&... args) : mHandler(function, mCommands, mResourceMutex, args...) {}

		/**
		 * Issue a new command to the worker.
		 *
		 * @param args The variadic arguments required to construct the command.
		 * @return The command base pointer of the created command.
		 */
		template<class Command, class... Args>
		CommandBase* IssueCommand(Args&&... args)
		{
			auto ptr = std::make_unique<Command>(std::forward<Args>(args)...);
			const auto rawPtr = ptr.get();

			auto guard = std::lock_guard(mResourceMutex);
			mCommands.emplace_back(std::move(ptr));

			return rawPtr;
		}

	protected:
		std::list<std::unique_ptr<CommandBase>> mCommands = {};
		std::mutex mResourceMutex = {};
		std::jthread mHandler;
	};
}