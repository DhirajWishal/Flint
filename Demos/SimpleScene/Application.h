// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Engine.h"
#include "Flint/Components/ShaderCode.h"

#include "Core/Maths/Matrix/Matrix44.h"
#include "Core/Maths/Vector/Vector3.h"

class Application {
	static float Dot(Flint::Vector3 lhs, Flint::Vector3 rhs);
	static Flint::Vector3 Cross(Flint::Vector3 lhs, Flint::Vector3 rhs);
	static Flint::Vector3 Normalize(Flint::Vector3 rhs);
	static Flint::Matrix44 LookAt(Flint::Vector3 const eye, Flint::Vector3 const center, Flint::Vector3 const up);
	static Flint::Matrix44 Perspective(float FOV, float aspect, float near, float far, bool flipYAxis = true);

	struct UniformBufferObject {
		Flint::Matrix44 model = Flint::Matrix44::Identity;
		Flint::Matrix44 view = Flint::Matrix44::Identity;
		Flint::Matrix44 projection = Perspective(45.0f * 0.01745329251994329576923690768489f, 1280.0f / 720.0f, 0.1f, 128.0f);
	} Ubo;

public:
	Application();
	~Application();

private:
	std::vector<Flint::ShaderDigest> CreateShaderDigests() const;
	void CreateSceneComponent();
	void PrepareToRender();

	void ProcessInputs();
	void OnUpdate();

	void DestroyRenderResources();

	Flint::Backend::GraphicsPipelineSpecification GetGraphicsPipelineSpec() const;

private:
	UI32 mWidth = 1280, mHeight = 720;
	Flint::Engine mEngine = {};

	Flint::WireFrame mWireFrame = {};
	Flint::SceneComponent mSceneComponent = {};
	Flint::RenderResource mRenderResource = {};
	Flint::Backend::DynamicStateContainer mDynamicState = {};

	Flint::Vector3 mPosition = Flint::Vector3::ZeroAll;

	Flint::Inputs::InputCenter* pInputCenter = nullptr;

	UniformBufferObject* pDataCopy = nullptr;
	Flint::Backend::Buffer* pUniformBuffer = nullptr;
};