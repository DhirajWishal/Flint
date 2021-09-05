// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Device.hpp"
#include "GraphicsCore/GeometryStore.hpp"
#include "GraphicsCore/ResourcePackager.hpp"

#include "Engine/Asset.hpp"

#include "Components/Camera.hpp"

#include <glm/glm.hpp>

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

	void Initialize(const std::shared_ptr<Flint::Device>& pDevice, const std::shared_ptr<Flint::ScreenBoundRenderTarget>& pScreenBoundRenderTarget, const Camera* pCamera);
	void Terminate();

	void SubmitToCommandBuffer(const std::shared_ptr<Flint::CommandBuffer>& pCommandBuffer);

private:
	void CreatePipeline();

	template<class Type>
	void SubmitToUniformBuffer(std::shared_ptr<Flint::Buffer> pBuffer, const Type& data)
	{
		Type* pDataStore = static_cast<Type*>(pBuffer->MapMemory(sizeof(Type)));
		*pDataStore = data;

		pBuffer->UnmapMemory();
	}

private:
	Flint::Asset mAsset;

	std::shared_ptr<Flint::Device> pDevice = nullptr;
	std::shared_ptr<Flint::ScreenBoundRenderTarget> pScreenBoundRenderTarget = nullptr;

	std::shared_ptr<Flint::GeometryStore> pGeometryStore = nullptr;

	std::shared_ptr<Flint::Shader> pVertexShader = nullptr;
	std::shared_ptr<Flint::Shader> pFragmentShader = nullptr;

	std::shared_ptr<Flint::GraphicsPipeline> pPipeline = nullptr;
	std::shared_ptr<Flint::ResourcePackager> pResourcePackager = nullptr;
	std::shared_ptr<Flint::ResourcePackage> pResourcePackage = nullptr;
	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;

	std::shared_ptr<Flint::Buffer> pUniformBuffer = nullptr;
	std::shared_ptr<Flint::Image> pTexture = nullptr;
	std::shared_ptr<Flint::ImageSampler> pTextureSampler = nullptr;

	const Camera* pCamera = nullptr;
};