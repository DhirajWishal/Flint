// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

namespace Flint
{
	/**
	 * Flint subsystem object.
	 * This object will carry out the graphical operations issued by the client applications on another thread.
	 */
	class FlintSubsystem
	{
	public:
		FlintSubsystem(Instance& instance) : mInstance(instance) {}
		~FlintSubsystem() { DestroyInstance(mInstance); }

	private:
		Instance& mInstance;
	};
}