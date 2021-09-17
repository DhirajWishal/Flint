// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/GraphicsPipeline.hpp"
#include "GraphicsCore/CommandBuffer.hpp"
#include "GraphicsCore/GeometryStore.hpp"
#include "GraphicsCore/ImageSampler.hpp"
#include "GraphicsCore/ResourcePackager.hpp"

#include <imgui.h>
#include <glm/glm.hpp>

namespace Flint
{
	struct ImGuiTextureContainer
	{
		std::shared_ptr<ResourcePackage> pResourcePackage = nullptr;
	};

	/**
	 * Im Gui adapter class.
	 * This class is responsible for rendering all the ImGui components.
	 */
	class ImGuiAdapter
	{
		struct PushConstants
		{
			glm::vec2 mScale = glm::vec2(1.0f);
			glm::vec2 mTranslate = glm::vec2(1.0f);
		} pushConstants;

	public:
		ImGuiAdapter();

		void Initialize(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget);

		void Render(const std::shared_ptr<CommandBuffer>& pCommandBuffer);
		void Terminate();

		std::shared_ptr<ResourcePackage> CreateResourcePackage() { return pResourcePackager->CreatePackage(); }

	private:
		void SetupImGui();
		void SetupGeometryStore();
		void SetupPipeline();
		void SetupImage();

		void UpdateGeometryStore();

	public:
		std::shared_ptr<Device> pDevice = nullptr;
		std::shared_ptr<ScreenBoundRenderTarget> pRenderTarget = nullptr;

		std::shared_ptr<GraphicsPipeline> pPipeline = nullptr;

		std::shared_ptr<Shader> pVertexShader = nullptr;
		std::shared_ptr<Shader> pFragmentShader = nullptr;

		std::shared_ptr<GeometryStore> pGeometryStore = nullptr;

		std::shared_ptr<Image> pFontImage = nullptr;
		std::shared_ptr<ImageSampler> pFontSampler = nullptr;

		std::shared_ptr<ResourcePackager> pResourcePackager = nullptr;
		std::shared_ptr<ResourcePackage> pResourcePack = nullptr;
		std::shared_ptr<DynamicStateContainer> pDynamicStateContainer = nullptr;

		ImDrawVert* pVertexData = nullptr;
		ImDrawIdx* pIndexData = nullptr;
	};
}