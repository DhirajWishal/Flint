// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Commands.hpp"

#include <mutex>
#include <condition_variable>

namespace Flint
{
	/**
	 * Backend callback type.
	 * This is the function type which will be passed to the backend thread when running.
	 *
	 * @param std::condition_variable& The conditional variable reference. This is used to block the thread until further notice.
	 * @param std::mutex& The mutex reference. This is used to lock the resources before writing to it.
	 * @param CommandStorage& The command storage reference. This contains all the commands that will be passed to the backend.
	 * @param Commands::CreateInstance&& The instance creation structure.
	 */
	using BackendCallback = void(std::condition_variable&, std::mutex&, CommandStorage&, Commands::CreateInstance&&);
}