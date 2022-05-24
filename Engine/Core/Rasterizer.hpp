// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"
#include "MeshRasterizer.hpp"

namespace Flint
{
	/**
	 * Rasterizer class.
	 * This class performs rasterization to the bound entities.
	 */
	class Rasterizer : public RenderTarget
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine reference.
		 * @param width The width of the render target.
		 * @param height The height of the render target.
		 * @param frameCount The number of frames in the render target. This is usually set automatically by the Window.
		 * @param attachmentDescriptions The attachment descriptions.
		 * @param multisample The multisample count. Default is One.
		 * @param exclusiveBuffering Whether or not to use one buffer/ attachment per frame. Default is false.
		 */
		explicit Rasterizer(Engine& engine, uint32_t width, uint32_t height, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample = Multisample::One, bool exclusiveBuffering = false)
			: RenderTarget(engine, width, height, frameCount, std::move(attachmentDescriptions), multisample, exclusiveBuffering) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~Rasterizer() = default;

		/**
		 * Register a geometry to the rasterizer.
		 * You can add the same geometry multiple times if different pipelines are being used.
		 *
		 * @param geometry The geometry to add.
		 * @param meshBinder The special callback that will be called on every single mesh in the geometry, and will be used to create the pipeline for each mesh.
		 * This is required as each mesh might need it's own pipeline because of the varying materials, inputs and so on. Note that if a pipeline exists for the same
		 * specification, a new one will not be created, the existing one will be used instead.
		 */
		virtual void registerGeometry(const Geometry& geometry, std::function<MeshRasterizer(const Mesh&, const Geometry&)>&& meshBinder) = 0;
	};
}