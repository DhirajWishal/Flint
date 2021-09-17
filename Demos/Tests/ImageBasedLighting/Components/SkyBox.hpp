// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TestBase/GameObject.hpp"
#include "Engine/Asset.hpp"
#include "GraphicsCore/ResourcePackager.hpp"

#include <glm/glm.hpp>

namespace Flint
{
	class SkyBox final : public GameObject
	{
		struct ModelViewProjection
		{
			ModelViewProjection() = default;
			ModelViewProjection(const glm::mat4& view, const glm::mat4& projection) : mViewMatrix(view), mProjectionMatrix(projection) {}
			ModelViewProjection(const CameraMatrix& matrix) : mViewMatrix(matrix.mViewMatrix), mProjectionMatrix(matrix.mProjectionMatrix) {}

			glm::mat4 mModelMatrix = glm::mat4(1);
			glm::mat4 mViewMatrix = glm::mat4(1);
			glm::mat4 mProjectionMatrix = glm::mat4(1);
			float mLODBias = 0.0f;
		} mMatrix;

	public:
		SkyBox() = default;

		virtual void Initialize(Application* pApplication) override final;
		virtual void Update(UI64 delta, Camera* pCamera) override final;
		virtual void Draw(const std::shared_ptr<CommandBuffer>& pCommandBuffer) override final;
		virtual void Terminate() override final;

	private:
		void CreatePipeline();
		void LoadAsset();
		void LoadTextures();

	private:
		Asset mAsset = {};

		Application* pApplication = nullptr;
		std::shared_ptr<GraphicsPipeline> pPipeline = nullptr;

		std::shared_ptr<ResourcePackager> pPackager = nullptr;
		std::shared_ptr<ResourcePackage> pPackage = nullptr;

		std::shared_ptr<Image> pImage = nullptr;
		std::shared_ptr<ImageSampler> pImageSampler = nullptr;

		std::shared_ptr<Buffer> pUniformBuffer = nullptr;

		std::shared_ptr<DynamicStateContainer> pDynamicStates = nullptr;
	};
}
