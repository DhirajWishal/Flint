// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Light.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "Flint/ImageSampler.hpp"

struct Vertex
{
	Vertex() = default;
	Vertex(glm::vec3 pos, glm::vec2 tex = {}) : position(pos), texture(tex) {}

	glm::vec3 position = {};
	glm::vec3 color = {};
	glm::vec2 texture = {};
};

std::pair<std::vector<Vertex>, std::vector<UI32>> CreateQuad()
{
	std::vector<Vertex> vertexes = {
		Vertex(glm::vec3(1.0f, 1.0f, 0.0f),		glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(1.0f, -1.0f, 0.0f),	glm::vec2(0.0f, 1.0f)),
		Vertex(glm::vec3(-1.0f, 1.0f, 0.0f),	glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(-1.0f, -1.0f, 0.0f),	glm::vec2(1.0f, 1.0f))
	};

	std::vector<UI32> indexes = {
		0, 1, 2,
		1, 3, 2
	};

	return { vertexes, indexes };
}

Light::Light(glm::vec3 position, SceneState* pSceneState)
	: GameObject(position, pSceneState)
{
	Flint::FBox2D windowExtent = pSceneState->pDisplay->GetExtent();

	pDynamicStates = std::make_shared<Flint::DynamicStateContainer>();
	pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
	pDynamicStates->SetScissor(windowExtent, { 0, 0 });

	if (pSceneState->pGraphicsPipelines.find("Default") == pSceneState->pGraphicsPipelines.end())
		pSceneState->CreateDefaultPipeline();

	auto image = LoadImage(pSceneState->GetAssetPath().string() + "\\Textures\\LightBulb\\outline_lightbulb_white_48dp.png");
	pTexture = pSceneState->pDevice->CreateImage(Flint::ImageType::DIMENSIONS_2, Flint::ImageUsage::GRAPHICS, image.mExtent, Flint::PixelFormat::R8G8B8A8_SRGB, 1, 1, image.pImageData);
	DestroyImage(image);

	pTextureSampler = pSceneState->pDevice->CreateImageSampler(Flint::ImageSamplerSpecification());

	pVertexShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::VERTEX, std::filesystem::path(pSceneState->GetAssetPath().string() + "\\Shaders\\Guizmo\\shader.vert.spv"));
	pFragmentShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::FRAGMENT, std::filesystem::path(pSceneState->GetAssetPath().string() + "\\Shaders\\Guizmo\\shader.frag.spv"));

	Flint::GraphicsPipelineSpecification specification = {};
	specification.mRasterizationSamples = pSceneState->pDevice->GetSupportedRasterizationSamples();
	specification.mDynamicStateFlags = Flint::DynamicStateFlags::VIEWPORT | Flint::DynamicStateFlags::SCISSOR;
	specification.mFrontFace = Flint::FrontFace::CLOCKWISE;

	pSceneState->pGraphicsPipelines["NoTexture"] = pSceneState->pDevice->CreateGraphicsPipeline("NoTexture", pSceneState->pScreenBoundRenderTargets["Default"], pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	pSceneState->pScreenBoundRenderTargets["Default"]->SubmitPipeline(pSceneState->pGeometryStores["Default"], pSceneState->pGraphicsPipelines["NoTexture"]);

	auto [vertexes, indexes] = CreateQuad();
	auto [vertexOffset, indexOffset] = pSceneState->pGeometryStores["Default"]->AddGeometry(vertexes.size(), vertexes.data(), indexes.size(), indexes.data());

	auto pResourceMap = pSceneState->pGraphicsPipelines["NoTexture"]->CreateResourceMap();
	pResourceMap->SetResource("ubo", pModelUniform);
	pResourceMap->SetResource("cam", pSceneState->mCamera.GetCameraBuffer());
	pResourceMap->SetResource("texSampler", pTextureSampler, pTexture);

	pSceneState->pGraphicsPipelines["NoTexture"]->AddDrawData(pResourceMap, pDynamicStates, vertexOffset, vertexes.size(), indexOffset, indexes.size());
}

Light::~Light()
{
	pSceneState->pDevice->DestroyShader(pVertexShader);
	pSceneState->pDevice->DestroyShader(pFragmentShader);

	pSceneState->pDevice->DestroyImage(pTexture);
	pSceneState->pDevice->DestroyImageSampler(pTextureSampler);
}

void Light::OnUpdate(UI64 delta)
{
	if (pSceneState->pDisplay->IsDisplayResized())
	{
		auto extent = pSceneState->pDisplay->GetExtent();
		if (!extent.IsZero())
		{
			pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(extent.mWidth), static_cast<float>(extent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
			pDynamicStates->SetScissor(extent, { 0, 0 });
		}
	}

	//auto [pitch, yaw] = pSceneState->mCamera.GetPitchYaw();
	//mModelMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(-pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	//mModelMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	//mModelMatrix *= glm::lookAt(mPosition, -pSceneState->mCamera.GetPosition(), pSceneState->mCamera.GetCameraUp());
	//auto xx = mModelMatrix[0][0];
	//auto yx = mModelMatrix[1][0];
	//auto zx = mModelMatrix[2][0];
	//
	//xx = std::sqrt((xx * xx) + (yx * yx) + (zx * zx));
	//mModelMatrix = glm::mat4(xx);
	//mModelMatrix[0][3] = mPosition.x;
	//mModelMatrix[1][3] = mPosition.y;
	//mModelMatrix[2][3] = mPosition.z;
	//mModelMatrix[3][3] = 1.0f;
	//
	//mModelMatrix *= glm::ortho()

	SubmitToUniformBuffer(pModelUniform, mModelMatrix);
}

void Light::SetPosition(glm::vec3 position)
{
	mPosition = position;
	mModelMatrix = glm::translate(glm::mat4(1.0f), mPosition);
}
