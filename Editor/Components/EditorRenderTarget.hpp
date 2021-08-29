// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/ScreenBoundRenderTarget.hpp"
#include "GraphicsCore/CommandBufferAllocator.hpp"

/**
 * Editor render target.
 */
class EditorRenderTarget
{
public:
	EditorRenderTarget() = default;

	void Initialize(const boost::shared_ptr<Flint::Device>& pDevice, const boost::shared_ptr<Flint::Instance>& pInstance);
	void Terminate();

	bool IsDisplayOpen() const;
	void PollEvents();
	void DrawFrame();

private:
	boost::container::vector<Flint::RenderTargetAttachment> mAttachments{ 2 };

	boost::shared_ptr<Flint::Display> pDisplay = nullptr;
	boost::shared_ptr<Flint::ScreenBoundRenderTarget> pRenderTarget = nullptr;

	boost::shared_ptr<Flint::CommandBufferAllocator> pAllocator = nullptr;
	boost::shared_ptr<Flint::CommandBufferAllocator> pSecondaryAllocator = nullptr;
};