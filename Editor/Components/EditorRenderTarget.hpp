// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/ScreenBoundRenderTarget.hpp"

/**
 * Editor render target.
 */
class EditorRenderTarget
{
public:
	EditorRenderTarget() {}
	~EditorRenderTarget() {}

	void Initialize(const std::shared_ptr<Flint::Device>& pDevice, const std::shared_ptr<Flint::Display>& pDisplay);

private:
	std::shared_ptr<Flint::ScreenBoundRenderTarget> pRenderTarget = nullptr;
};