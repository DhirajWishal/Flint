// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Backend/ScreenBoundRenderTarget.hpp"
#include "Backend/GraphicsPipeline.hpp"
#include "Backend/GeometryStore.hpp"

#include "Camera.hpp"

struct SceneState
{
	SceneState();
	~SceneState();

	void PrepareRenderTargetsToDraw();
	void PrepareNewFrame();
	void SubmitFrames();

	void UpdateCamera();

public:

	Camera mCamera = {};

	std::unordered_map<std::string, std::shared_ptr<Flint::ScreenBoundRenderTarget>> pScreenBoundRenderTargets;
	std::unordered_map<std::string, std::shared_ptr<Flint::GraphicsPipeline>> pGraphicsPipelines;
	std::unordered_map<std::string, std::shared_ptr<Flint::GeometryStore>> pGeometryStores;

	std::shared_ptr<Flint::Instance> pInstance = nullptr;
	std::shared_ptr<Flint::Display> pDisplay = nullptr;
	std::shared_ptr<Flint::Device> pDevice = nullptr;

private:
	void CreateDefaultRenderTarget();
	void CreateDefaultPipeline();
};