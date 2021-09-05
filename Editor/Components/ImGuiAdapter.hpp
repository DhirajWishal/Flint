// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/GraphicsPipeline.hpp"
#include "GraphicsCore/CommandBuffer.hpp"
#include "GraphicsCore/GeometryStore.hpp"
#include "GraphicsCore/ImageSampler.hpp"

#include <imgui.h>
#include <glm/glm.hpp>

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

	void Initialize(const std::shared_ptr<Flint::Device>& pDevice, const std::shared_ptr<Flint::ScreenBoundRenderTarget>& pRenderTarget);
	void Render(const std::shared_ptr<Flint::CommandBuffer>& pCommandBuffer);
	void Terminate();

private:
	void SetupImGui();
	void SetupGeometryStore();
	void SetupPipeline();
	void SetupImage();

	void UpdateGeometryStore();

public:
	std::shared_ptr<Flint::Device> pDevice = nullptr;
	std::shared_ptr<Flint::ScreenBoundRenderTarget> pRenderTarget = nullptr;

	std::shared_ptr<Flint::GraphicsPipeline> pPipeline = nullptr;

	std::shared_ptr<Flint::Shader> pVertexShader = nullptr;
	std::shared_ptr<Flint::Shader> pFragmentShader = nullptr;

	std::shared_ptr<Flint::GeometryStore> pGeometryStore = nullptr;

	std::shared_ptr<Flint::Image> pFontImage = nullptr;
	std::shared_ptr<Flint::ImageSampler> pFontSampler = nullptr;

	std::shared_ptr<Flint::ResourcePackager> pResourcePackager = nullptr;
	std::shared_ptr<Flint::ResourcePackage> pResourcePack = nullptr;
	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStateContainer;
};