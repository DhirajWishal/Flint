// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/OffScreenRenderTarget.hpp"
#include "GraphicsCore/ScreenBoundRenderTarget.hpp"
#include "GraphicsCore/CommandBufferAllocator.hpp"

#include "Engine/ClientInterface.hpp"

#include "SceneRenderTarget.hpp"

#include "Demos/VikingRoom.hpp"

namespace Flint
{
	/**
	 * Editor render target.
	 */
	class EditorRenderTarget
	{
	public:
		EditorRenderTarget();
		~EditorRenderTarget();

		void Initialize(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Instance>& pInstance);
		void Terminate();

		bool IsDisplayOpen() const;
		void PollEvents(UI64 delta);
		void DrawFrame(const ClientInterface* pClient);

		void UpdateUI(const UI64 delta);

		const bool HasDragAndDrop() const { return pDisplay->ContainsDragAndDrop(); }
		const std::vector<std::filesystem::path> GetDropPaths();

		void SetExtent(const FBox2D extent) { mExtent = extent; }
		const FBox2D GetExtent() const { return mExtent; }
		const std::shared_ptr<ScreenBoundRenderTarget> GetRenderTarget() const { return pRenderTarget; }

	private:
		FBox2D mExtent = {};

		std::vector<RenderTargetAttachment> mAttachments{ 2 };

		std::shared_ptr<Display> pDisplay = nullptr;

		std::shared_ptr<ScreenBoundRenderTarget> pRenderTarget = nullptr;

		std::shared_ptr<CommandBufferAllocator> pAllocator = nullptr;
		std::shared_ptr<CommandBufferAllocator> pSecondaryAllocator = nullptr;

		ImGuiAdapter mImGuiAdapter = {};
		SceneRenderTarget mSceneRenderTarget = {};
	};
}