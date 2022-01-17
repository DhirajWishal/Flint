// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "TestBase/Nodes/OffScreenPass.hpp"
#include "Graphics/Objects/Asset.hpp"
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

		struct EyeMatrix
		{
			glm::mat4 mProjectionMatrix = glm::mat4(1.0f);
			glm::mat4 mViewMatrix = glm::mat4(1.0f);
		} mLeftEyeMat, mRightEyeMat;

	public:
		Object() = default;
		Object(Application* pApplication, std::shared_ptr<OffScreenPass> pOffScreenPass);

		virtual void Initialize(Application* pApplication) override;
		virtual void Update(uint64 delta, Camera* pCamera) override;
		virtual void Draw(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const uint32 index) override;
		virtual void OcclusionPass(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const uint32 index) override;
		virtual void ResetOcclusionQuery(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const uint32 index, const bool isFirstUse) override;
		virtual void Terminate() override;

	private:
		void CreatePipeline();
		void CreateOcclusionPipeline();
		void LoadAsset();
		void LoadTextures();

	private:
		glm::mat4 mLeftEyeView = glm::mat4(1.0f);
		glm::mat4 mRightEyeView = glm::mat4(1.0f);

		Asset mAsset = {};
		std::vector<std::vector<std::shared_ptr<ResourcePackage>>> pPackageSets = {};
		std::vector<std::shared_ptr<ResourcePackage>> pOcclusionPackageSets = {};
		std::vector<std::shared_ptr<Query>> pOcclusionQueries = {};
		std::vector<std::vector<uint64>> mDrawSamples = {};

		Application* pApplication = nullptr;
		std::shared_ptr<OffScreenPass> pOffScreenPass = nullptr;
		std::shared_ptr<GraphicsPipeline> pPipeline = nullptr;
		std::shared_ptr<GraphicsPipeline> pOcclusionPipeline = nullptr;

		std::shared_ptr<Buffer> pUniformBuffer = nullptr;

		std::shared_ptr<DynamicStateContainer> pDynamicStates = nullptr;
		std::shared_ptr<DynamicStateContainer> pDynamicStatesLeftEye = nullptr;
		std::shared_ptr<DynamicStateContainer> pDynamicStatesRightEye = nullptr;

		std::future<bool> mAsyncStatus;
		bool bFirstAsync = true;

		float mEyeExpansion = 0.0f;

		bool bShouldFreezeOcclusion = false;
		bool bSkipCulling = false;
		bool bStereoVision = true;
	};
}
