// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Types\DataTypes.h"

namespace Flint
{
	enum class ShaderResourceType : UI8 {
		UNDEFINED, BUFFER, SAMPLER
	};

	class FShaderResource {
	public:
		FShaderResource(UI32 binding, UI32 layer, ShaderResourceType type) : mBindingLocation(binding), mLayerIndex(layer), mResourceType(type) {}

		UI32 GetBindingLocation() const { return mBindingLocation; }
		UI32 GetLayerIndex() const { return mLayerIndex; }
		ShaderResourceType GetResourceType() const { return mResourceType; }

	protected:
		UI32 mBindingLocation = 0;
		UI32 mLayerIndex = 0;
		ShaderResourceType mResourceType = ShaderResourceType::UNDEFINED;
	};

	class FDrawBuffer : public FShaderResource {
	public:

	};
}