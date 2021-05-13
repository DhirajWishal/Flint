// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.h"
#include "Buffer.h"

namespace Flint
{
	namespace Backend
	{
		enum class ShaderResourceType : UI8 {
			UNDEFINED, BUFFER, SAMPLER
		};

		class ShaderResource : public DeviceBoundObject
		{
		public:
			ShaderResource(Device* pDevice, UI32 binding, UI32 layer, ShaderResourceType type) : DeviceBoundObject(pDevice), mBindingLocation(binding), mLayerIndex(layer), mResourceType(type) {}

			UI32 GetBindingLocation() const { return mBindingLocation; }
			UI32 GetLayerIndex() const { return mLayerIndex; }
			ShaderResourceType GetResourceType() const { return mResourceType; }

		protected:
			UI32 mBindingLocation = 0;
			UI32 mLayerIndex = 0;
			ShaderResourceType mResourceType = ShaderResourceType::UNDEFINED;
		};
	}
}