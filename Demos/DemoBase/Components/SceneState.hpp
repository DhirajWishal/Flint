// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/ScreenBoundRenderTarget.hpp"
#include "Flint/GraphicsPipeline.hpp"
#include "Flint/GeometryStore.hpp"

#include "Camera.hpp"

struct SceneState
{
	SceneState(const std::string& displayTitle);
	~SceneState();

	void PrepareRenderTargetsToDraw();
	void PrepareNewFrame();
	void SubmitFrames();

	void UpdateCamera(UI64 delta);

	const boost::filesystem::path GetSolutionPath() const { return mSolutionPath; }
	const boost::filesystem::path GetAssetPath() const { return mAssetPath; }

public:

	Camera mCamera = {};

	boost::unordered::unordered_map<std::string, boost::shared_ptr<Flint::ScreenBoundRenderTarget>> pScreenBoundRenderTargets;
	boost::unordered::unordered_map<std::string, boost::shared_ptr<Flint::OffScreenRenderTarget>> pOffScreenRenderTargets;
	boost::unordered::unordered_map<std::string, boost::shared_ptr<Flint::GraphicsPipeline>> pGraphicsPipelines;
	boost::unordered::unordered_map<std::string, boost::shared_ptr<Flint::ComputePipeline>> pComputePipelines;
	boost::unordered::unordered_map<std::string, boost::shared_ptr<Flint::GeometryStore>> pGeometryStores;

	boost::filesystem::path mSolutionPath = "";
	boost::filesystem::path mAssetPath = "";

	boost::shared_ptr<Flint::Instance> pInstance = nullptr;
	boost::shared_ptr<Flint::Display> pDisplay = nullptr;
	boost::shared_ptr<Flint::Device> pDevice = nullptr;

	boost::shared_ptr<Flint::Shader> pVertexShader = nullptr;
	boost::shared_ptr<Flint::Shader> pFragmentShader = nullptr;

	boost::shared_ptr<Flint::Shader> pVertexShaderBB = nullptr;
	boost::shared_ptr<Flint::Shader> pFragmentShaderBB = nullptr;

	void CreateDefaultPipeline();

private:
	void CreateDefaultRenderTarget();
	void CreateBoundingBoxPipeline();
};