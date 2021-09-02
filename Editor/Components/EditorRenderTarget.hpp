// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ScreenBoundRenderTarget.hpp"
#include "GraphicsCore/CommandBufferAllocator.hpp"

#include "ImGuiAdapter.hpp"

/**
 * Editor render target.
 */
class EditorRenderTarget
{
public:
	EditorRenderTarget() = default;

	void Initialize(const std::shared_ptr<Flint::Device>& pDevice, const std::shared_ptr<Flint::Instance>& pInstance);
	void Terminate();

	bool IsDisplayOpen() const;
	void PollEvents();
	void DrawFrame();

private:
	std::vector<Flint::RenderTargetAttachment> mAttachments{ 2 };

	std::shared_ptr<Flint::Display> pDisplay = nullptr;
	std::shared_ptr<Flint::ScreenBoundRenderTarget> pRenderTarget = nullptr;

	std::shared_ptr<Flint::CommandBufferAllocator> pAllocator = nullptr;
	std::shared_ptr<Flint::CommandBufferAllocator> pSecondaryAllocator = nullptr;

	ImGuiAdapter mImGuiAdapter = {};
};