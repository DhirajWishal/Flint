// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VikingRoom.hpp"
#include "Flint/OffScreenRenderTargetFactory.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include <imgui.h>
#include <ImGuizmo.h>

VikingRoom::VikingRoom(glm::vec3 position, SceneState* pSceneState) : GameObject(position, pSceneState)
{
	pDynamicStates = std::make_shared<Flint::DynamicStateContainer>();
	pUniformBufferObject = pSceneState->pDevice->CreateBuffer(Flint::BufferType::Uniform, sizeof(UniformBufferObject));

	auto image = LoadImage(pSceneState->GetAssetPath().string() + "\\Packages\\VikingRoom\\VikingRoom\\texture.png");
	pTexture = pSceneState->pDevice->CreateImage(Flint::ImageType::TwoDimension, Flint::ImageUsage::Graphics, image.mExtent, Flint::PixelFormat::R8G8B8A8_SRGB, 1, 1, image.pImageData);
	DestroyImage(image);

	pTextureSampler = pSceneState->pDevice->CreateImageSampler(Flint::ImageSamplerSpecification());

	if (pSceneState->pGraphicsPipelines.find("DefaultWireframe") == pSceneState->pGraphicsPipelines.end())
		SetupPipeline();

	auto pPipeline = pSceneState->pGraphicsPipelines["DefaultWireframe"];
	pResourceMap = pPipeline->CreateResourceMap();

	pResourceMap->SetResource("Ubo", pModelUniform);
	pResourceMap->SetResource("Camera", pSceneState->mCamera.GetCameraBuffer());
	pResourceMap->SetResource("texSampler", pTextureSampler, pTexture);

	Flint::FBox2D windowExtent = pSceneState->pDisplay->GetExtent();
	pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
	pDynamicStates->SetScissor(windowExtent, { 0, 0 });

	auto asset = ImportAsset(pSceneState->pDevice, pSceneState->GetAssetPath().string() + "\\Packages\\VikingRoom\\VikingRoom\\vikingroom.fbx");
	auto [vertexOffset, indexOffset] = pSceneState->pGeometryStores["Default"]->AddGeometry(asset.pVertexBuffer, asset.pIndexBuffer);
	for (auto instance : asset.mDrawInstances)
	{
		mDrawIndex = pPipeline->AddDrawData(pResourceMap, pDynamicStates, vertexOffset + instance.mVertexOffset, instance.mVertexCount, indexOffset + instance.mIndexOffset, instance.mIndexCount);
		mVertexCount += instance.mVertexCount;
		mIndexCount += instance.mIndexCount;

		auto pResourceMapMP = pSceneState->pGraphicsPipelines["MousePicker"]->CreateResourceMap();
		pResourceMapMP->SetResource("Ubo", pUniformBufferObject);
		pResourceMapMP->SetResource("Camera", pSceneState->mCamera.GetCameraBuffer());
		pResourceMapMP->SetResource("texSampler", pTextureSampler, pTexture);
		pSceneState->pGraphicsPipelines["MousePicker"]->AddDrawData(pResourceMapMP, pDynamicStates, vertexOffset + instance.mVertexOffset, instance.mVertexCount, indexOffset + instance.mIndexOffset, instance.mIndexCount);
	}

	mVertexOffset = vertexOffset;
	mIndexOffset = indexOffset;

	asset.pVertexBuffer->Terminate();
	asset.pIndexBuffer->Terminate();

	SetupBoundingBox();
}

VikingRoom::~VikingRoom()
{
	pSceneState->pGeometryStores["Default"]->RemoveGeometry(mVertexOffset, mVertexCount, mIndexOffset, mIndexCount);
	pSceneState->pGraphicsPipelines["DefaultWireframe"]->RemoveDrawData(mDrawIndex);

	pTexture->Terminate();
	pTextureSampler->Terminate();

	pVertexShaderMP->Terminate();
	pFragmentShaderMP->Terminate();

	pUniformBufferObject->Terminate();
}

void VikingRoom::OnUpdate(UI64 delta)
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

	// Rotate x
	if (pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KeyX).IsPressed() || pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KeyX).IsOnRepeat())
	{
		if ((pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KeyX).GetSpecialCharacter() & Flint::SpecialCharacter::Shift) == Flint::SpecialCharacter::Shift)
			mModelMatrix *= glm::rotate(glm::mat4(1.0f), mRotationBias, glm::vec3(1.0f, 0.0f, 0.0f));
		else
			mModelMatrix *= glm::rotate(glm::mat4(1.0f), -mRotationBias, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	// Rotate y
	if (pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KeyY).IsPressed() || pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KeyY).IsOnRepeat())
	{
		if ((pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KeyY).GetSpecialCharacter() & Flint::SpecialCharacter::Shift) == Flint::SpecialCharacter::Shift)
			mModelMatrix *= glm::rotate(glm::mat4(1.0f), mRotationBias, glm::vec3(0.0f, 1.0f, 0.0f));
		else
			mModelMatrix *= glm::rotate(glm::mat4(1.0f), -mRotationBias, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	// Rotate z
	if (pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KeyZ).IsPressed() || pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KeyZ).IsOnRepeat())
	{
		if ((pSceneState->pDisplay->GetKeyEvent(Flint::KeyCode::KeyZ).GetSpecialCharacter() & Flint::SpecialCharacter::Shift) == Flint::SpecialCharacter::Shift)
			mModelMatrix *= glm::rotate(glm::mat4(1.0f), mRotationBias, glm::vec3(0.0f, 0.0f, 1.0f));
		else
			mModelMatrix *= glm::rotate(glm::mat4(1.0f), -mRotationBias, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	auto pos = pSceneState->pDisplay->GetMousePosition();
	if ((pSceneState->pOffScreenRenderTargets["MousePicker"]->GetResult(0)->GetPixelValue<float>(Flint::FBox3D(static_cast<UI32>(pos.mWidth), static_cast<UI32>(pos.mHeight), 0)) == mUniformBufferObject.mIndex) && pSceneState->pDisplay->GetMouseButtonEvent(Flint::MouseButton::Left).IsPressed())
		EnableBoundingBox();

	if (pSceneState->pDisplay->GetMouseButtonEvent(Flint::MouseButton::Right).IsPressed())
		DisableBoundingBox();

	if (bIsBoundingBoxEnabled)
	{
		auto mat = pSceneState->mCamera.GetMatrix();
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::UNIVERSAL);

		auto pDisplay = pSceneState->pDisplay;
		if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyOne).IsPressed())
			mCurrentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		else if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyTwo).IsPressed())
			mCurrentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
		else if (pDisplay->GetKeyEvent(Flint::KeyCode::KeyThree).IsPressed())
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
	else
		ImGuizmo::SetRect(0, 0, 0, 0);

	// Submit data to uniforms.
	mUniformBufferObject.mModel = mModelMatrix;
	SubmitToUniformBuffer(pModelUniform, mModelMatrix);
	SubmitToUniformBuffer(pUniformBufferObject, mUniformBufferObject);
	UpdateBoundingBox();
}

void VikingRoom::SetupPipeline()
{
	Flint::GraphicsPipelineSpecification specification = {};
	specification.mRasterizationSamples = pSceneState->pDevice->GetSupportedMultiSampleCount();
	specification.mDynamicStateFlags = Flint::DynamicStateFlags::ViewPort | Flint::DynamicStateFlags::Scissor;
	specification.bEnableDepthTest = true;
	specification.bEnableDepthWrite = true;
	specification.mColorBlendConstants[0] = 0.0f;
	specification.mColorBlendConstants[1] = 0.0f;
	specification.mColorBlendConstants[2] = 0.0f;
	specification.mColorBlendConstants[3] = 0.0f;
	//specification.mPrimitiveTopology = Flint::PrimitiveTopology::TriangleList;
	//specification.mPolygonMode = Flint::PolygonMode::Line;

	pSceneState->pGraphicsPipelines["DefaultWireframe"] = pSceneState->pDevice->CreateGraphicsPipeline("DefaultWireframe", pSceneState->pScreenBoundRenderTargets["Default"], pSceneState->pVertexShader, nullptr, nullptr, nullptr, pSceneState->pFragmentShader, specification);
	pSceneState->pScreenBoundRenderTargets["Default"]->SubmitGraphicsPipeline(pSceneState->pGeometryStores["Default"], pSceneState->pGraphicsPipelines["DefaultWireframe"]);

	// Create off screen render target.
	pSceneState->pOffScreenRenderTargets["MousePicker"] = pSceneState->pDevice->CreateOffScreenRenderTargetFactory()->Create(Flint::OffScreenRenderTargetType::MOUSE_PICKER, pSceneState->pDisplay->GetExtent(), 3);
	pSceneState->pScreenBoundRenderTargets["Default"]->AttachOffScreenRenderTarget(pSceneState->pOffScreenRenderTargets["MousePicker"]);

	// Prepare pipeline.
	pVertexShaderMP = pSceneState->pDevice->CreateShader(Flint::ShaderType::Vertex, std::filesystem::path("Flint\\Shaders\\MousePicking3D\\shader.vert.spv"));
	pFragmentShaderMP = pSceneState->pDevice->CreateShader(Flint::ShaderType::Fragment, std::filesystem::path("Flint\\Shaders\\MousePicking3D\\shader.frag.spv"));

	specification.mRasterizationSamples = Flint::MultiSampleCount::One;
	pSceneState->pGraphicsPipelines["MousePicker"] = pSceneState->pDevice->CreateGraphicsPipeline("MousePicker", pSceneState->pOffScreenRenderTargets["MousePicker"], pVertexShaderMP, nullptr, nullptr, nullptr, pFragmentShaderMP, specification);
	pSceneState->pOffScreenRenderTargets["MousePicker"]->SubmitGraphicsPipeline(pSceneState->pGeometryStores["Default"], pSceneState->pGraphicsPipelines["MousePicker"]);
}