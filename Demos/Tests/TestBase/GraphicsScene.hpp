// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Application.hpp"
#include "ImGuiAdapter.hpp"
#include "Camera.hpp"

#include "GraphicsCore/ScreenBoundRenderTarget.hpp"
#include "GraphicsCore/GraphicsPipeline.hpp"
#include "Engine/Asset.hpp"
#include "GameObject.hpp"

namespace Flint
{
	struct DrawDataContainer
	{
		DrawDataContainer() = default;
		DrawDataContainer(const std::shared_ptr<GraphicsPipeline>& pPipeline, const Asset& asset) : pPipeline(pPipeline), mAsset(asset) {}

		Asset mAsset = {};
		std::shared_ptr<GraphicsPipeline> pPipeline = nullptr;
	};

	class GraphicsScene
	{
	public:
		GraphicsScene() = default;
		GraphicsScene(Application* pApplication, FBox2D extent);

		void Terminate();

		std::shared_ptr<Display> GetDisplay() const { return pRenderTarget->GetDisplay(); }
		std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(const std::string& name, const std::shared_ptr<Shader>& pVertexShader, const std::shared_ptr<Shader>& pFragmentShader, GraphicsPipelineSpecification spec);
		std::shared_ptr<GraphicsPipeline> GetGraphicsPipeline(const std::string& name) const;

		void SubmitAssetsToDraw(const std::string& name, const std::shared_ptr<GraphicsPipeline>& pPipeline, const Asset& asset);
		void SubmitGameObject(GameObject* pObject) { pGameObjects.push_back(pObject); }

		void Update();
		void DrawFrame();

	public:
		Camera mCamera = {};
		std::chrono::steady_clock mClock = {};
		std::chrono::time_point<std::chrono::steady_clock> mOldTimePoint = {};

		std::vector<GameObject*> pGameObjects = {};

		std::shared_ptr<Device> pDevice = nullptr;

		std::shared_ptr<ScreenBoundRenderTarget> pRenderTarget = nullptr;
		std::shared_ptr<CommandBufferAllocator> pAllocator = nullptr;
		std::vector<std::shared_ptr<CommandBuffer>> pCommandBuffers = {};

		std::shared_ptr<CommandBufferAllocator> pSecondaryAllocator = nullptr;
		std::vector<std::shared_ptr<CommandBuffer>> pSecondaryCommandBuffers = {};

		std::unordered_map<std::string, std::shared_ptr<GraphicsPipeline>> pGraphicsPipelines = {};
		std::unordered_map<std::string, DrawDataContainer> mDrawData = {};

		ImGuiAdapter mImGuiAdapter = {};
	};
}