// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "LowLevel/Device.h"

namespace Flint
{
	namespace LowLevel
	{
		/**
		 * Flint Graphics Pipeline object.
		 * This pipeline uses rasterization to render to a render target.
		 */
		class GraphicsPipeline {
		public:
			GraphicsPipeline() {}
			~GraphicsPipeline() {}

		public:
			Interface::GraphicsPipelineHandle mHandle = FLINT_NULL_HANDLE;
		};
	}
}