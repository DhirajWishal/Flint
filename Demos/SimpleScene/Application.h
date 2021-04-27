// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint\Objects\FInstance.h"
#include "Flint\Objects\FDevice.h"
#include "Flint\Objects\FDisplay.h"
#include "Flint\Objects\FScreenBoundRenderTarget.h"
#include "Flint\Objects\FShader.h"
#include "Flint\Objects\FGraphicsPipeline.h"

#include "Flint\Components\WireFrame.h"

#include "Core/Maths/Matrix/Matrix44.h"
#include "Core/Maths/Vector/Vector3.h"

class Application {
	static float Dot(Flint::Vector3 lhs, Flint::Vector3 rhs);
	static Flint::Vector3 Cross(Flint::Vector3 lhs, Flint::Vector3 rhs);
	static Flint::Vector3 Normalize(Flint::Vector3 rhs);
	static Flint::Matrix44 LookAt(Flint::Vector3 const eye, Flint::Vector3 const center, Flint::Vector3 const up);
	static Flint::Matrix44 Perspective(float FOV, float aspect, float near, float far, bool flipYAxis = true);

	const Flint::Vector3 Front = Flint::Vector3(1.0f, 0.0f, 0.0f);
	const Flint::Vector3 Left = Flint::Vector3(0.0f, 0.0f, 1.0f);
	const float MoveFactor = 0.001f;

	struct UniformBufferObject {
		Flint::Matrix44 model = Flint::Matrix44::Identity;
		Flint::Matrix44 view = Flint::Matrix44::Identity;
		Flint::Matrix44 projection = Perspective(45.0f * 0.01745329251994329576923690768489f, 1280.0f / 720.0f, 0.1f, 128.0f);
	} Ubo;

public:
	Application();
	~Application();

private:
	void SetupShaders();
	void CreateSceneComponent();
	void PrepareToRender();

	void ProcessInputs();
	void OnUpdate();

	void DestroyRenderResources();

	Flint::GraphicsPipelineSpecification GetGraphicsPipelineSpec() const;

private:
	Flint::FInstance mInstance = {};
	Flint::FDisplay mDisplay = {};
	Flint::FDevice mDevice = {};

	Flint::FScreenBoundRenderTarget mRenderTarget = {};

	Flint::FShader mShaders[2] = {};
	Flint::FGraphicsPipeline mPipeline = {};

	Flint::WireFrame mWireFrame = {};

	Flint::FDrawIndex mDrawIndex = FLINT_NULL_HANDLE;
};