// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Reactor/Reactor.hpp"

namespace Flint
{
	void VulkanReactorWorker(std::list<std::unique_ptr<CommandBase>>& commands, std::mutex& mutex);
}