// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Light.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "Flint/ImageSampler.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

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
	pSceneState->pScreenBoundRenderTargets["Default"]->SubmitGraphicsPipeline(pSceneState->pGeometryStores["Default"], pSceneState->pGraphicsPipelines["NoTexture"]);

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
	pVertexShader->Terminate();
	pFragmentShader->Terminate();

	pTexture->Terminate();
	pTextureSampler->Terminate();
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

	{
		auto mat = pSceneState->mCamera.GetMatrix();
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::UNIVERSAL);

		auto pDisplay = pSceneState->pDisplay;
		if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_1).IsPressed())
			mCurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		else if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_2).IsPressed())
			mCurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
		else if (pDisplay->GetKeyEvent(Flint::KeyCode::KEY_3).IsPressed())
			mCurrentGizmoOperation = ImGuizmo::OPERATION::SCALE;

		float* matrix = &mModelMatrix[0].x;
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Transform", matrixTranslation);
		ImGui::InputFloat3("Rotation", matrixRotation);
		ImGui::InputFloat3("Scale", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);
		mPosition = glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);

		// To fix inverted y axis.
		mat.mViewMatrix[0][1] = -mat.mViewMatrix[0][1];
		mat.mViewMatrix[1][1] = -mat.mViewMatrix[1][1];
		mat.mViewMatrix[2][1] = -mat.mViewMatrix[2][1];
		mat.mViewMatrix[3][1] = -mat.mViewMatrix[3][1];

		float* cameraView = &mat.mViewMatrix[0].x;
		float* cameraProjection = &mat.mProjectionMatrix[0].x;

		ImGuiIO& io = ImGui::GetIO();
		float viewManipulateRight = io.DisplaySize.x;
		float viewManipulateTop = io.DisplaySize.y;

		ImGuizmo::SetRect(0, 0, viewManipulateRight, viewManipulateTop);
		ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, NULL, NULL, NULL);
	}

	SubmitToUniformBuffer(pModelUniform, mModelMatrix);
}
