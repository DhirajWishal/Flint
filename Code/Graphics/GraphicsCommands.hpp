// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Reactor/CommandBase.hpp"

#define FLINT_CREATE_IDENTIFIER(name)	enum class name : uint64 {}

namespace Flint
{
	namespace GraphicsCommands
	{
		/**
		 * Create a new device command.
		 */
		class CreateDevice final : public CommandBase
		{
		public:
			/**
			 * Default constructor.
			 */
			CreateDevice() = default;
		};
	}
}