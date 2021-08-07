// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/ScreenBoundRenderTarget.hpp"
#include "Flint/GraphicsPipeline.hpp"
#include "Flint/GeometryStore.hpp"

#include "Camera.hpp"

struct SceneState
{
	SceneState();
	~SceneState();

	void PrepareRenderTargetsToDraw();
	void PrepareNewFrame();
	void SubmitFrames();

	void UpdateCamera(UI64 delta);

	const std::filesystem::path GetSolutionPath() const { return mSolutionPath; }
	const std::filesystem::path GetAssetPath() const { return mAssetPath; }

public:

	Camera mCamera = {};

	std::unordered_map<std::string, std::shared_ptr<Flint::ScreenBoundRenderTarget>> pScreenBoundRenderTargets;
	std::unordered_map<std::string, std::shared_ptr<Flint::GraphicsPipeline>> pGraphicsPipelines;
	std::unordered_map<std::string, std::shared_ptr<Flint::GeometryStore>> pGeometryStores;

	std::filesystem::path mSolutionPath = "";
	std::filesystem::path mAssetPath = "";

	std::shared_ptr<Flint::Instance> pInstance = nullptr;
	std::shared_ptr<Flint::Display> pDisplay = nullptr;
	std::shared_ptr<Flint::Device> pDevice = nullptr;

	std::shared_ptr<Flint::Shader> pVertexShader = nullptr;
	std::shared_ptr<Flint::Shader> pFragmentShader = nullptr;

	std::shared_ptr<Flint::Shader> pVertexShaderBB = nullptr;
	std::shared_ptr<Flint::Shader> pFragmentShaderBB = nullptr;

	void CreateDefaultPipeline();

private:
	void CreateDefaultRenderTarget();
	void CreateBoundingBoxPipeline();
};