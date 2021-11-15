// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ComponentBase.hpp"

namespace Flint
{
	namespace Components
	{
		struct DrawData
		{
			DrawData(const GraphicsCore::WireFrame* pWireFrame, const std::vector<std::shared_ptr<Graphics::ResourcePackage>>& pPackages, const std::shared_ptr<GraphicsCore::DynamicStateContainer>& pDynamicStates)
				: pWireFrame(pWireFrame), pResourcePackages(pPackages), pDynamicStateContainer(pDynamicStates) {}

			const GraphicsCore::WireFrame* pWireFrame = nullptr;
			std::vector<std::shared_ptr<Graphics::ResourcePackage>> pResourcePackages = {};
			std::shared_ptr<GraphicsCore::DynamicStateContainer> pDynamicStateContainer = nullptr;
		};

		/**
		 * Draw instance graphics structure.
		 */
		struct DrawInstanceGraphics
		{
			DrawInstanceGraphics(const std::shared_ptr<Graphics::GeometryStore>& pGeometryStore, const std::shared_ptr<Graphics::GraphicsPipeline>& pPipeline)
				: pGeometryStore(pGeometryStore), pGraphicsPipeline(pPipeline) {}

			void AddDrawData(const GraphicsCore::WireFrame* pWireFrame, const std::vector<std::shared_ptr<Graphics::ResourcePackage>>& pPackages, const std::shared_ptr<GraphicsCore::DynamicStateContainer>& pDynamicStates)
			{
				mDrawData.emplace_back(DrawData(pWireFrame, pPackages, pDynamicStates));
			}

			std::vector<DrawData> mDrawData = {};
			std::shared_ptr<Graphics::GeometryStore> pGeometryStore = nullptr;
			std::shared_ptr<Graphics::GraphicsPipeline> pGraphicsPipeline = nullptr;
		};
	}
}