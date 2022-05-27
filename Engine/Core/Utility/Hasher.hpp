// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Rasterizer.hpp"
#include "Core/Geometry.hpp"

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
	 * Generate hash for a mesh.
	 *
	 * @param mesh The mesh to generate the hash.
	 * @return The hash value.
	 */
	[[nodiscard]] uint64_t GenerateHash(const Mesh& mesh);

	/**
	 * Generate hash for the vertex descriptor.
	 *
	 * @param descriptor The vertex descriptor.
	 * @return The hash.
	 */
	[[nodiscard]] uint64_t GenerateHash(const VertexDescriptor& descriptor);
}