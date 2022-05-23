// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/MeshRasterizer.hpp"

namespace Flint
{
	/**
	 * Generate hash for the resource binding table.
	 *
	 * @param table The resource binding table.
	 * @return The hash value.
	 */
	[[nodiscard]] uint64_t GenerateHash(const ResourceBindingTable& table);

	/**
	 * Generate hash for the rasterizing pipeline specification.
	 * Note that this operation is quite costly.
	 *
	 * @param specification The pipeline specification.
	 * @return The hash value.
	 */
	[[nodiscard]] uint64_t GenerateHash(const RasterizingPipelineSpecification& specification);

	/**
	 * Generate hash for the mesh rasterizer.
	 * Note that this operation is quite costly.
	 *
	 * @param meshRasterizer The mesh rasterizer to generate the hash.
	 * @return The hash value.
	 */
	[[nodiscard]] uint64_t GenerateHash(const MeshRasterizer& meshRasterizer);
}