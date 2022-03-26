// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/GraphicsDevice.hpp"

namespace Flint
{
	GraphicsDevice::GraphicsDevice(GraphicsEngine* pEngine)
	{
	}

	GraphicsDevice::~GraphicsDevice()
	{
		pDevice->Terminate();
	}
}