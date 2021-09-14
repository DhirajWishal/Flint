// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Application.hpp"
#include "GraphicsCore/ScreenBoundRenderTarget.hpp"
#include "GraphicsCore/GraphicsPipeline.hpp"

namespace Flint
{
	class GraphicsScene
	{
	public:
		GraphicsScene(ApplicationState* pState, const FBox2D extent);
		~GraphicsScene();

		std::shared_ptr<GraphicsPipeline> CreateGraphicsPipeline(const std::string& name, const std::shared_ptr<Shader>& pVertexShader, const std::shared_ptr<Shader>& pFragmentShader, GraphicsPipelineSpecification spec);
		std::shared_ptr<GraphicsPipeline> GetGraphicsPipeline(const std::string& name) const;
	public:
		std::shared_ptr<Device> pDevice = nullptr;

		std::shared_ptr<ScreenBoundRenderTarget> pRenderTarget = nullptr;
		std::shared_ptr<CommandBufferAllocator> pAllocator = nullptr;
		std::vector<std::shared_ptr<CommandBuffer>> pCommandBuffers = {};

		std::unordered_map<std::string, std::shared_ptr<GraphicsPipeline>> pGraphicsPipelines = {};
	};
}