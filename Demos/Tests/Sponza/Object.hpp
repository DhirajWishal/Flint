// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TestBase/GameObject.hpp"
#include "Engine/Asset.hpp"
#include "GraphicsCore/ResourcePackager.hpp"

#include <glm/glm.hpp>

namespace Flint
{
	class Object final : public GameObject
	{
		struct ModelViewProjection
		{
			ModelViewProjection() = default;
			ModelViewProjection(const glm::mat4& view, const glm::mat4& projection) : mViewMatrix(view), mProjectionMatrix(projection) {}
			ModelViewProjection(const CameraMatrix& matrix) : mViewMatrix(matrix.mViewMatrix), mProjectionMatrix(matrix.mProjectionMatrix) {}

			glm::mat4 mModelMatrix = glm::mat4(1);
			glm::mat4 mViewMatrix = glm::mat4(1);
			glm::mat4 mProjectionMatrix = glm::mat4(1);
			float mMipBias = 0.0f;
		} mMatrix;

	public:
		Object() = default;

		virtual void Initialize(Application* pApplication) override;
		virtual void Update(UI64 delta, Camera* pCamera) override;
		virtual void Draw(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const UI32 index) override;
		virtual void Terminate() override;

	private:
		void CreatePipeline();
		void LoadAsset();
		void LoadTextures();

	private:
		Asset mAsset = {};
		std::vector<std::vector<std::shared_ptr<ResourcePackage>>> pPackageSets = {};

		Application* pApplication = nullptr;
		std::shared_ptr<GraphicsPipeline> pPipeline = nullptr;

		std::shared_ptr<Buffer> pUniformBuffer = nullptr;

		std::shared_ptr<DynamicStateContainer> pDynamicStates = nullptr;
	};
}
