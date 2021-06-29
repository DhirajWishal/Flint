// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "BackendWrapper.hpp"
#include "Instance.hpp"
#include "Device.hpp"
#include "Display.hpp"

namespace Flint
{
	using FInstance = BackendWrapper<Backend::Instance>;
	using FDevice = BackendWrapper<Backend::Device>;
	using FDisplay = BackendWrapper<Backend::Display>;
}