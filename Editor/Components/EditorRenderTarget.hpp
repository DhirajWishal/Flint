// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/OffScreenRenderTarget.hpp"
#include "GraphicsCore/ScreenBoundRenderTarget.hpp"
#include "GraphicsCore/CommandBufferAllocator.hpp"

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
		EditorRenderTarget() = default;

		void Initialize(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Instance>& pInstance);
		void Terminate();

		bool IsDisplayOpen() const;
		void PollEvents(UI64 delta);
		void DrawFrame();

		void UpdateUI(const UI64 delta);

	private:
		std::shared_ptr<Display> pDisplay = nullptr;

		std::shared_ptr<ScreenBoundRenderTarget> pRenderTarget = nullptr;

		std::shared_ptr<CommandBufferAllocator> pAllocator = nullptr;
		std::shared_ptr<CommandBufferAllocator> pSecondaryAllocator = nullptr;

		ImGuiAdapter mImGuiAdapter = {};
		SceneRenderTarget mSceneRenderTarget = {};
	};
}