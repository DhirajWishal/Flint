// Copyright 2021-2022 Dhiraj Wishal
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
			DrawInstanceGraphics(const std::shared_ptr<GeometryStore>& pGeometryStore, GraphicsPipeline* pPipeline)
				: pGeometryStore(pGeometryStore), pGraphicsPipeline(pPipeline) {}

			void AddDrawData(const WireFrame* pWireFrame, const std::vector<std::shared_ptr<ResourcePackage>>& pPackages, const std::shared_ptr<DynamicStateContainer>& pDynamicStates)
			{
				m_DrawData.emplace_back(DrawData(pWireFrame, pPackages, pDynamicStates));
			}

			std::vector<DrawData> m_DrawData = {};
			std::shared_ptr<GeometryStore> pGeometryStore = nullptr;
			std::unique_ptr<GraphicsPipeline> pGraphicsPipeline = nullptr;
		};
	}
}