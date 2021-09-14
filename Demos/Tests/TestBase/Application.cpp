// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Application.hpp"

namespace Flint
{
	Application::Application()
	{
		mState.pInstance = CreateInstance(true);
		mState.pDevice = mState.pInstance->CreateDevice(DeviceFlags::External | DeviceFlags::GraphicsCompatible | DeviceFlags::ComputeCompatible);
	}

	Application::~Application()
	{

	}
}