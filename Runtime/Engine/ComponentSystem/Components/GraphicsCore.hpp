// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/Abstraction/AbstractionObject.hpp"
#include "GraphicsCore/Device.hpp"

namespace Flint
{
	namespace Components
	{
		/**
		 * Graphics instance component.
		 */
		struct GraphicsInstance
		{
			GraphicsInstance(const bool enableValidation);

			AbstractionObject mInstance = {};
		};

		/**
		 * Graphics device component.
		 */
		struct GraphicsDevice
		{
			GraphicsDevice(GraphicsInstance* pInstance, const GraphicsCore::DeviceFlags flags);

			AbstractionObject mDevice = {};
		};

		/**
		 * Buffer component.
		 */
		struct Buffer
		{
			Buffer(GraphicsDevice* pDevice, const GraphicsCore::BufferType type, const uint64 size, const GraphicsCore::BufferMemoryProfile profile = GraphicsCore::BufferMemoryProfile::Automatic);

			AbstractionObject mBuffer = {};
		};

		/**
		 * Staging buffer component.
		 */
		struct StagingBuffer final : public Buffer { StagingBuffer(GraphicsDevice* pDevice, const uint64 size, const GraphicsCore::BufferMemoryProfile profile = GraphicsCore::BufferMemoryProfile::Automatic) : Buffer(pDevice, GraphicsCore::BufferType::Staging, size, profile) {} };
		
		/**
		 * Vertex buffer component.
		 */
		struct VertexBuffer final : public Buffer { VertexBuffer(GraphicsDevice* pDevice, const uint64 size, const GraphicsCore::BufferMemoryProfile profile = GraphicsCore::BufferMemoryProfile::Automatic) : Buffer(pDevice, GraphicsCore::BufferType::Vertex, size, profile) {} };
		
		/**
		 * Index buffer component.
		 */
		struct IndexBuffer final : public Buffer { IndexBuffer(GraphicsDevice* pDevice, const uint64 size, const GraphicsCore::BufferMemoryProfile profile = GraphicsCore::BufferMemoryProfile::Automatic) : Buffer(pDevice, GraphicsCore::BufferType::Index, size, profile) {} };
		
		/**
		 * Uniform buffer component.
		 */
		struct UniformBuffer final : public Buffer { UniformBuffer(GraphicsDevice* pDevice, const uint64 size, const GraphicsCore::BufferMemoryProfile profile = GraphicsCore::BufferMemoryProfile::Automatic) : Buffer(pDevice, GraphicsCore::BufferType::Uniform, size, profile) {} };
		
		/**
		 * Storage buffer component.
		 */
		struct StorageBuffer final : public Buffer { StorageBuffer(GraphicsDevice* pDevice, const uint64 size, const GraphicsCore::BufferMemoryProfile profile = GraphicsCore::BufferMemoryProfile::Automatic) : Buffer(pDevice, GraphicsCore::BufferType::Storage, size, profile) {} };

		/**
		 * Shader component.
		 */
		struct Shader
		{
			Shader(GraphicsDevice* pDevice, const GraphicsCore::ShaderType type, const std::filesystem::path& assetFile);
			Shader(GraphicsDevice* pDevice, const GraphicsCore::ShaderType type, const std::string& code);
			Shader(GraphicsDevice* pDevice, const GraphicsCore::ShaderType type, const std::vector<uint32>& code);

			AbstractionObject mShader = {};
		};

		/**
		 * Geometry store component.
		 */
		struct GeometryStore
		{
			GeometryStore(GraphicsDevice* pDevice, const GraphicsCore::TShaderAttributeMap& vertexAttributes, uint64 indexSize, const GraphicsCore::BufferMemoryProfile profile = GraphicsCore::BufferMemoryProfile::Automatic);

			AbstractionObject mGeometryStore = {};
		};
	}
}