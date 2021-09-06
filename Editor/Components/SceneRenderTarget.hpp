// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/OffScreenRenderTarget.hpp"
#include "GraphicsCore/CommandBufferAllocator.hpp"

#include "ImGuiAdapter.hpp"
#include "Demos/VikingRoom.hpp"

namespace Flint
{
	class SceneRenderTarget
	{
	public:
		SceneRenderTarget() = default;

		void Initialize(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount, const FBox2D extent, const std::shared_ptr<ResourcePackage>& pPackage);
		void Terminate();

		void Render(const std::shared_ptr<Display>& pDisplay, const UI64 delta);

		void Recreate(const FBox2D& extent);

	private:
		const FBox2D GetExtent() const { return { static_cast<UI32>(mExtent.x), static_cast<UI32>(mExtent.y) }; }
		void UpdateCamera(const std::shared_ptr<Display>& pDisplay, const UI64 delta);

	private:
		VikingRoom mVikingRoom = {};
		Camera mCamera = {};

		std::vector<RenderTargetAttachment> mAttachments{ 2 };

		std::shared_ptr<OffScreenRenderTarget> pRenderTarget = nullptr;

		std::shared_ptr<CommandBufferAllocator> pAllocator = nullptr;
		std::shared_ptr<CommandBufferAllocator> pSecondaryAllocator = nullptr;
		std::shared_ptr<ImageSampler> pSampler = nullptr;

		ImGuiTextureContainer mTextureContainer = {};

		ImVec2 mExtent = ImVec2(848, 480);
	};
}