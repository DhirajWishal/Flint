// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Device.hpp"
#include "GraphicsCore/GeometryStore.hpp"
#include "GraphicsCore/ResourcePackager.hpp"

#include "Engine/Asset.hpp"

#include "Components/Camera.hpp"

#include <glm/glm.hpp>

namespace Flint
{
	class VikingRoom
	{
		struct UniformBufferObject
		{
			glm::mat4 mModelMatrix = glm::mat4(1.0f);
			glm::mat4 mViewMatrix = glm::mat4(1.0f);
			glm::mat4 mProjectionMatrix = glm::mat4(1.0f);
		} mUniformBufferObject;

	public:
		VikingRoom() = default;

		void Initialize(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget, const Camera* pCamera);
		void Initialize(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget, const Camera* pCamera);
		void Recreate();
		void Terminate();

		void SubmitToCommandBuffer(const std::shared_ptr<CommandBuffer>& pCommandBuffer);

	private:
		void CreatePipeline(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget);
		void CreatePipeline(const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget);

		template<class Type>
		void SubmitToUniformBuffer(std::shared_ptr<Buffer> pBuffer, const Type& data)
		{
			Type* pDataStore = static_cast<Type*>(pBuffer->MapMemory(sizeof(Type)));
			*pDataStore = data;

			pBuffer->UnmapMemory();
		}

	private:
		Asset mAsset;

		std::shared_ptr<Device> pDevice = nullptr;
		std::shared_ptr<RenderTarget> pRenderTarget = nullptr;

		std::shared_ptr<GeometryStore> pGeometryStore = nullptr;

		std::shared_ptr<Shader> pVertexShader = nullptr;
		std::shared_ptr<Shader> pFragmentShader = nullptr;

		std::shared_ptr<GraphicsPipeline> pPipeline = nullptr;
		std::shared_ptr<ResourcePackager> pResourcePackager = nullptr;
		std::shared_ptr<ResourcePackage> pResourcePackage = nullptr;
		std::shared_ptr<DynamicStateContainer> pDynamicStates = nullptr;

		std::shared_ptr<Buffer> pUniformBuffer = nullptr;
		std::shared_ptr<Image> pTexture = nullptr;
		std::shared_ptr<ImageSampler> pTextureSampler = nullptr;

		const Camera* pCamera = nullptr;
	};
}