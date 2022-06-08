// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Rasterizer.hpp"

namespace Flint
{
	/**
	 * Generate hash for the resource binding table.
	 *
	 * @param table The resource binding table.
	 * @return The hash value.
	 */
	[[nodiscard]] uint64_t GenerateHash(const Core::ResourceBindingTable& table);

	/**
	 * Generate hash for the rasterizing pipeline specification.
	 * Note that this operation is quite costly.
	 *
	 * @param specification The pipeline specification.
	 * @return The hash value.
	 */
	[[nodiscard]] uint64_t GenerateHash(const Core::RasterizingPipelineSpecification& specification);

	/**
	 * Generate hash for the vertex descriptor.
	 *
	 * @param descriptor The vertex descriptor.
	 * @return The hash.
	 */
	[[nodiscard]] uint64_t GenerateHash(const Core::VertexDescriptor& descriptor);
}