// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Instance.hpp"
#include "GraphicsCore/Device.hpp"

namespace Flint
{
	struct ApplicationState
	{
		std::shared_ptr<Instance> pInstance = nullptr;
		std::shared_ptr<Device> pDevice = nullptr;

		std::unordered_map<std::string, std::shared_ptr<GeometryStore>> pGeometryStores;
	};

	class Application
	{
	public:
		Application();
		~Application();

		ApplicationState mState = {};
	};
}
