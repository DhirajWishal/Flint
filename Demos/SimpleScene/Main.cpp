// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Benchmark/Timer.h"
#include "Flint/Engine.h"
#include "Flint/Components/ShaderCode.h"

#include "Core/Maths/Matrix/Matrix44.h"
#include "Core/Maths/Vector/Vector3.h"

#include <thread>
#include <math.h>

std::vector<Flint::ShaderDigest> GetShaders()
{
	Flint::ShaderCode shaders[2] = {};
	shaders[0].LoadFromFile("E:\\Flint\\Assets\\Shaders\\TestShaders\\Default\\shader.vert.spv", Flint::ShaderLocation::VERTEX, Flint::ShaderCodeType::SPIR_V);
	shaders[1].LoadFromFile("E:\\Flint\\Assets\\Shaders\\TestShaders\\Default\\shader.frag.spv", Flint::ShaderLocation::FRAGMENT, Flint::ShaderCodeType::SPIR_V);

	return { shaders[0].CreateDigest(), shaders[1].CreateDigest() };
}

float dot(Flint::Vector3 lhs, Flint::Vector3 rhs)
{
	Flint::Vector3 _tmp(lhs * rhs);
	return _tmp.x + _tmp.y + _tmp.z;
}

Flint::Vector3 cross(Flint::Vector3 lhs, Flint::Vector3 rhs)
{
	return (Flint::Vector3(lhs.y, lhs.z, lhs.x) * Flint::Vector3(rhs.z, rhs.x, rhs.y)) -
		(Flint::Vector3(rhs.y, rhs.z, rhs.x) * Flint::Vector3(lhs.z, lhs.x, lhs.y));
}

Flint::Vector3 normalize(Flint::Vector3 rhs)
{
	return rhs * (1.0f / std::sqrt(dot(rhs, rhs)));
}

static Flint::Matrix44 lookAt(Flint::Vector3 const eye, Flint::Vector3 const center, Flint::Vector3 const up)
{
	auto const f = normalize(center - eye);
	auto const s = normalize(cross(f, up));
	auto const u = cross(s, f);

	Flint::Matrix44 newMatrix(1.0f);
	newMatrix[0][0] = s.x;
	newMatrix[1][0] = s.y;
	newMatrix[2][0] = s.z;
	newMatrix[3][0] = -dot(s, eye);

	newMatrix[0][1] = u.x;
	newMatrix[1][1] = u.y;
	newMatrix[2][1] = u.z;
	newMatrix[3][1] = -dot(u, eye);

	newMatrix[0][2] = -f.x;
	newMatrix[1][2] = -f.y;
	newMatrix[2][2] = -f.z;
	newMatrix[3][2] = dot(f, eye);

	return newMatrix;
}

static Flint::Matrix44 perspective(float FOV, float aspect, float near, float far, bool flipYAxis = true)
{
	float const halfTan = tan(FOV / 2.0f);

	Flint::Matrix44 newMatrix(0.0f);
	newMatrix[0][0] = 1.0f / (aspect * halfTan);
	newMatrix[1][1] = 1.0f / (halfTan);
	newMatrix[2][2] = far / (near - far);
	newMatrix[2][3] = -1.0f;
	newMatrix[3][2] = -(far * near) / (far - near);

	if (flipYAxis)
		newMatrix[1][1] *= -1.0f;

	return newMatrix;
}

int main()
{
	Flint::Benchmark::Timer t;
	Flint::Engine engine = {};
#ifdef FLINT_DEBUG
	engine.Initialize(Flint::BackendAPI::VULKAN, true, 1280, 720, "Flint: Sample Scene");

#else
	engine.Initialize(Flint::BackendAPI::VULKAN, false, 1280, 720, "Flint: Sample Scene");

#endif // FLINT_DEBUG

	engine.CreateRenderTarget(Flint::Vector2(1280, 720));

	auto wireFrame = engine.CreateNewWireFrame("E:\\Flint\\Assets\\Models\\SkyBox\\SkySphere.obj", Flint::CreateDefaultAttributes());

	std::vector<Flint::ShaderDigest> digests = std::move(GetShaders());
	Flint::SceneComponent sceneComponent = engine.CreateSceneComponent(wireFrame, digests, Flint::GraphicsPipelinePresets::CreateDefaultSpec());
	Flint::RenderResource resource = engine.SubmitToDrawQueue(sceneComponent);

	engine.PrepareRenderTargetToRender();
	Flint::Inputs::InputCenter* pInputCenter = engine.GetDisplay()->GetInputCenter();

	Flint::Vector3 position = Flint::Vector3::ZeroAll;

	struct {
		Flint::Matrix44 model = Flint::Matrix44::Identity;
		Flint::Matrix44 view = Flint::Matrix44::Identity;
		Flint::Matrix44 projection = perspective(45.0f * 0.01745329251994329576923690768489f, 1280.0f / 720.0f, 0.1f, 128.0f);
	} Ubo;

	Flint::Backend::Buffer* pUniformBuffer = resource.mUniformBuffers["Ubo"];

	while (pInputCenter->IsWindowOpen)
	{
		if (!engine.BeginUpdate()) break;

		void* pData = pUniformBuffer->MapMemory(std::numeric_limits<UI32>::max(), 0);

		if (pInputCenter->KeyW.IsPressed() || pInputCenter->KeyW.IsOnRepeat())
			position += Flint::Vector3(1.0f, 0.0f, 0.0f) * 0.001f;

		if (pInputCenter->KeyS.IsPressed() || pInputCenter->KeyS.IsOnRepeat())
			position -= Flint::Vector3(1.0f, 0.0f, 0.0f) * 0.001f;

		if (pInputCenter->KeyA.IsPressed() || pInputCenter->KeyA.IsOnRepeat())
			position += Flint::Vector3(0.0f, 0.0f, 1.0f) * 0.001f;

		if (pInputCenter->KeyD.IsPressed() || pInputCenter->KeyD.IsOnRepeat())
			position -= Flint::Vector3(0.0f, 0.0f, 1.0f) * 0.001f;

		Ubo.view = lookAt(position, position + Flint::Vector3(1.0f, 0.0f, 0.0f), Flint::Vector3(0.0f, 1.0f, 0.0f));

		std::memcpy(pData, &Ubo, sizeof(Flint::Matrix44) * 3);

		pUniformBuffer->FlushMemoryMappings();
		pUniformBuffer->UnmapMemory();

		engine.EndUpdate();
	}

	sceneComponent.DestroyRenderResources(resource);
	engine.DestroySceneComponent(sceneComponent);

	engine.Terminate();
	return 0;
}
