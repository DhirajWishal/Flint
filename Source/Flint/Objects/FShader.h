// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FDevice.h"

namespace Flint
{
	/**
	 * Flint shader object.
	 * This object holds information about a single shader object.
	 */
	class FShader final : public FObject {
	public:
		FShader();
		~FShader();

		/**
		 * Initialize the shader.
		 *
		 * @param device: The device object which the shader is bound to.
		 * @param digest: The shader digest.
		 */
		void Initialize(const FDevice& device, const ShaderDigest& digest);

		/**
		 * Terminate the shader.
		 */
		void Terminate();
	};
}