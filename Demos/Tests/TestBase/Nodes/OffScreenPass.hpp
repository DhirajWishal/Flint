// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/ProcessingPipeline/Nodes/ProcessingNode.hpp"
#include "GraphicsCore/OffScreenRenderTarget.hpp"
#include "TestBase/GameObject.hpp"
#include "TestBase/ImGuiAdapter.hpp"

namespace Flint
{
	/**
	 * Off screen pass object.
	 * This object renders object to an off screen render target.
	 */
	class OffScreenPass final : public ProcessingNode
	{
	public:
		/**
		 * Constructor.
		 *
		 * @param pProcessingPipeline The processing pipeline pointer.
		 */
		OffScreenPass(ProcessingPipeline* pProcessingPipeline);

		/**
		 * Process function override.
		 *
		 * @param pCommandBuffer The command buffer pointer.
		 * @param frameIndex The frame index.
		 * @param imageIndex THe current image index.
		 */
		virtual void Process(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const UI32 frameIndex, const UI32 imageIndex) override;

	public:
		/**
		 * Create a new game object and add it to the list.
		 *
		 * @param args The constructor arguments.
		 * @return The created game object pointer.
		 */
		template<class Type, class... Args>
		std::shared_ptr<Type> CreateGameObject(const Args&...  args)
		{
			std::shared_ptr<Type> pGameObject = std::make_shared<Type>(args...);
			pGameObjects.emplace_back(pGameObject);
			return pGameObject;
		}

	public:
		/**
		 * Get the off screen render target.
		 *
		 * @return The render target pointer.
		 */
		std::shared_ptr<OffScreenRenderTarget> GetRenderTarget() const { return pOffScreenRenderTarget; }

		/**
		 * Get the created game objects.
		 *
		 * @return The vector of game objects.
		 */
		std::vector<std::shared_ptr<GameObject>> GetGameObjects() const { return pGameObjects; }

		/**
		 * Get the color image of the off screen render pass.
		 *
		 * @return The color image.
		 */
		std::shared_ptr<Image> GetColorImage() const { return pOffScreenRenderTarget->GetAttachment(0).pImage; }

		/**
		 * Get the depth image of the off screen render pass.
		 *
		 * @return The depth image.
		 */
		std::shared_ptr<Image> GetDepthImage() const { return pOffScreenRenderTarget->GetAttachment(1).pImage; }

	private:
		/**
		 * Create the required attachments.
		 *
		 * @return The render target attachments.
		 */
		std::vector<RenderTargetAttachment> CreateAttachments() const;

	private:
		ImGuiAdapter mImGuiAdapter = {};

		std::vector<std::shared_ptr<GameObject>> pGameObjects = {};
		std::shared_ptr<OffScreenRenderTarget> pOffScreenRenderTarget = nullptr;
	};
}
