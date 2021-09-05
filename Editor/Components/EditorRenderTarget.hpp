// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ScreenBoundRenderTarget.hpp"
#include "GraphicsCore/CommandBufferAllocator.hpp"

#include "Camera.hpp"
#include "ImGuiAdapter.hpp"

#include "Demos/VikingRoom.hpp"

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
	void PollEvents(UI64 delta);
	void DrawFrame();

	void UpdateUI();

private:
	Camera mCamera;

	std::vector<Flint::RenderTargetAttachment> mAttachments{ 2 };

	std::shared_ptr<Flint::Display> pDisplay = nullptr;
	std::shared_ptr<Flint::ScreenBoundRenderTarget> pRenderTarget = nullptr;

	std::shared_ptr<Flint::CommandBufferAllocator> pAllocator = nullptr;
	std::shared_ptr<Flint::CommandBufferAllocator> pSecondaryAllocator = nullptr;

	ImGuiAdapter mImGuiAdapter = {};
	VikingRoom mVikingRoom = {};
};