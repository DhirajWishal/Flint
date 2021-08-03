// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DataType.hpp"
#include <queue>

namespace Flint
{
	namespace Thread
	{
		struct CommandBase
		{
			CommandBase(UI64 commandID) : mCommandID(commandID) {}

			const UI64 GetID() const { return mCommandID; }

			template<class Type>
			Type* StaticCast() { return static_cast<Type*>(this); }

			template<class Type>
			const Type* StaticCast() const { return static_cast<const Type*>(this); }

			UI64 mCommandID = 0;
		};

		/**
		 * Flint thread command center.
		 * This object is used to submit commands another thread with minimal overhead.
		 */
		class CommandCenter
		{
		public:
			CommandCenter() = default;
			CommandCenter(UI64 maxCommandQueueLength) : mMaxCommandQueueLength(maxCommandQueueLength) {}

			bool AddCommand(std::unique_ptr<CommandBase>&& pCommand);

			std::unique_ptr<CommandBase> GetCommand();

			const UI64 GetCount() const { return mMaxCommandQueueLength; }

			void SetMaxCommandQueueLength(UI64 maxCommandQueueLength) { mMaxCommandQueueLength = maxCommandQueueLength; }

		private:
			std::queue<std::unique_ptr<CommandBase>> pCommandQueue;
			UI64 mMaxCommandQueueLength = 10;
		};
	}
}