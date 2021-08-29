// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Device.hpp"

#include "Components/EditorRenderTarget.hpp"

/**
 * Main application class of the editor.
 */
class Application
{
public:
	Application();
	~Application();

private:
	void Execute();

private:
	boost::shared_ptr<Flint::Instance> pInstance = nullptr;
	boost::shared_ptr<Flint::Device> pDevice = nullptr;

	EditorRenderTarget mRenderTarget;

	bool bShouldRun = true;
};