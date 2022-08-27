// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Instance.hpp"

namespace Flint
{
	/**
	 * Create a new instance.
	 *
	 * @param applicationName The name of the application.
	 * @param applicationVersion The version of the application.
	 * @param enableBackendValidation Whether or not to enable backend validation. Turn this to true ONLY in the debug build.
	 * @return The created instance pointer.
	 */
	[[nodiscard]] std::shared_ptr<Backend::Instance> CreateInstance(std::string&& applicationName, uint32_t applicationVersion, bool enableBackendValidation);
}