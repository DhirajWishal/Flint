// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ComponentBase.hpp"
#include "GraphicsCore/ResourcePackage.hpp"
#include "GraphicsCore/DynamicStateContainer.hpp"
#include "GraphicsCore/WireFrame.hpp"
#include "GraphicsCore/GeometryStore.hpp"
#include "GraphicsCore/GraphicsPipeline.hpp"

namespace Flint
{
	namespace Components
	{
		struct DrawData
		{
			DrawData() = default;
			DrawData(const WireFrame* pWireFrame, const std::vector<std::shared_ptr<ResourcePackage>>& pPackages, const std::shared_ptr<DynamicStateContainer>& pDynamicStates)
				: pWireFrame(pWireFrame), pResourcePackages(pPackages), pDynamicStateContainer(pDynamicStates) {}

			const WireFrame* pWireFrame = nullptr;
			std::vector<std::shared_ptr<ResourcePackage>> pResourcePackages = {};
			std::shared_ptr<DynamicStateContainer> pDynamicStateContainer = nullptr;
		};

		/**
		 * Draw instance graphics structure.
		 */
		struct DrawInstanceGraphics
		{
			DrawInstanceGraphics() = default;
			DrawInstanceGraphics(const std::shared_ptr<GeometryStore>& pGeometryStore, const std::shared_ptr<GraphicsPipeline>& pPipeline)
				: pGeometryStore(pGeometryStore), pGraphicsPipeline(pPipeline) {}

			void AddDrawData(const WireFrame* pWireFrame, const std::vector<std::shared_ptr<ResourcePackage>>& pPackages, const std::shared_ptr<DynamicStateContainer>& pDynamicStates)
			{
				mDrawData.push_back(DrawData(pWireFrame, pPackages, pDynamicStates));
			}

			std::vector<DrawData> mDrawData = {};
			std::shared_ptr<GeometryStore> pGeometryStore = nullptr;
			std::shared_ptr<GraphicsPipeline> pGraphicsPipeline = nullptr;
		};
	}
}