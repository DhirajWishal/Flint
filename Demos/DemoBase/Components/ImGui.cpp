// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ImGui.hpp"

#include <iostream>

ImGUI::ImGUI(glm::vec3 position, SceneState* pSceneState)
	: GameObject(position, pSceneState)
{
	ImGui::CreateContext();
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
	style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(static_cast<float>(pSceneState->pDisplay->GetExtent().mWidth), static_cast<float>(pSceneState->pDisplay->GetExtent().mHeight));
	io.DisplayFramebufferScale = ImVec2(16.0f, 9.0f);

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	pVertexShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::Vertex, std::filesystem::path("Flint\\Shaders\\ImGui\\UI.vert.spv"));
	pFragmentShader = pSceneState->pDevice->CreateShader(Flint::ShaderType::Fragment, std::filesystem::path("Flint\\Shaders\\ImGui\\UI.frag.spv"));

	PrepareGeometryStore();
	PreparePipeline();
	PrepareImage();
}

ImGUI::~ImGUI()
{
	pTextImage->Terminate();
	pTextImageSampler->Terminate();

	pVertexShader->Terminate();
	pFragmentShader->Terminate();

	ImGui::DestroyContext();
}

void ImGUI::OnUpdate(UI64 delta)
{
	UpdateBuffers();

	ImGuiIO& io = ImGui::GetIO();
	ImDrawData* pDrawData = ImGui::GetDrawData();

	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	pushConstants.mScale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
	pushConstants.mTranslate = glm::vec2(-1.0f);

	if (pDrawData->CmdListsCount)
	{
		pDynamicStateContainers.clear();
		pPipeline->ClearDrawData();
		UI64 vertexOffset = 0, indexOffset = 0;
		for (I32 i = 0; i < pDrawData->CmdListsCount; i++)
		{
			auto pCommandList = pDrawData->CmdLists[i];

			for (I32 j = 0; j < pCommandList->CmdBuffer.Size; j++)
			{
				auto& pCommand = pCommandList->CmdBuffer[j];

				std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = std::make_shared<Flint::DynamicStateContainer>();
				pDynamicStates->SetViewPort(Flint::FExtent2D<float>(io.DisplaySize.x, io.DisplaySize.y), Flint::FExtent2D<float>(0.0f, 1.0f), Flint::FExtent2D<float>());

				pDynamicStates->SetScissor(
					Flint::FBox2D(static_cast<UI32>(pCommand.ClipRect.z - pCommand.ClipRect.x), static_cast<UI32>(pCommand.ClipRect.w - pCommand.ClipRect.y)),
					Flint::FBox2D(std::max(static_cast<I32>(pCommand.ClipRect.x), 0), std::max(static_cast<I32>(pCommand.ClipRect.y), 0)));

				pDynamicStates->SetConstantData(Flint::ShaderType::Vertex, &pushConstants, sizeof(PushConstants));

				pPipeline->AddDrawData(pResourceMap, pDynamicStates, vertexOffset, 0, indexOffset, pCommand.ElemCount);

				indexOffset += pCommand.ElemCount;
				pDynamicStateContainers.push_back(pDynamicStates);
			}

			vertexOffset += pCommandList->VtxBuffer.Size;
		}
	}
	else
	{
		for (auto pDynamicState : pDynamicStateContainers)
		{
			pDynamicState->SetViewPort(Flint::FExtent2D<float>(io.DisplaySize.x, io.DisplaySize.y), Flint::FExtent2D<float>(0.0f, 1.0f), Flint::FExtent2D<float>());
			pDynamicState->SetConstantData(Flint::ShaderType::Vertex, &pushConstants, sizeof(PushConstants));
		}
	}
}

void ImGUI::PrepareGeometryStore()
{
	pSceneState->pGeometryStores["ImGUI"] = pSceneState->pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(UI16), Flint::BufferMemoryProfile::TransferFriendly);
}

void ImGUI::PreparePipeline()
{
	Flint::GraphicsPipelineSpecification specification = {};
	specification.mRasterizationSamples = pSceneState->pDevice->GetSupportedMultiSampleCount();
	specification.mDynamicStateFlags = Flint::DynamicStateFlags::ViewPort | Flint::DynamicStateFlags::Scissor;
	specification.mCullMode = Flint::CullMode::None;
	specification.mColorBlendAttachments[0].mEnableBlend = true;
	specification.mColorBlendAttachments[0].mSrcBlendFactor = Flint::ColorBlendFactor::SourceAlpha;
	specification.mColorBlendAttachments[0].mDstBlendFactor = Flint::ColorBlendFactor::OneMinusSourceAlpha;
	specification.mColorBlendAttachments[0].mSrcAlphaBlendFactor = Flint::ColorBlendFactor::OneMinusSourceAlpha;
	specification.mColorBlendAttachments[0].mDstAlphaBlendFactor = Flint::ColorBlendFactor::Zero;
	specification.mDepthCompareLogic = Flint::DepthCompareLogic::LessOrEqual;

	specification.mColorBlendConstants[0] = 0.0f;
	specification.mColorBlendConstants[1] = 0.0f;
	specification.mColorBlendConstants[2] = 0.0f;
	specification.mColorBlendConstants[3] = 0.0f;

	specification.bEnableSampleShading = false;
	specification.mMinSampleShading = 0.0f;

	pPipeline = pSceneState->pDevice->CreateGraphicsPipeline("ImGUI", pSceneState->pScreenBoundRenderTargets["Default"], pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	pSceneState->pScreenBoundRenderTargets["Default"]->SubmitVolatilePipeline(pSceneState->pGeometryStores["ImGUI"], pPipeline);
	pSceneState->pGraphicsPipelines["ImGUI"] = pPipeline;
}

void ImGUI::PrepareImage()
{
	ImGuiIO& imGuiIO = ImGui::GetIO();

	unsigned char* pFontImage = nullptr;
	int width = 0, height = 0, bitsPerPixel = 0;

	imGuiIO.Fonts->GetTexDataAsRGBA32(&pFontImage, &width, &height, &bitsPerPixel);
	pTextImage = pSceneState->pDevice->CreateImage(Flint::ImageType::TwoDimension, Flint::ImageUsage::Graphics, Flint::FBox3D(width, height, 1), Flint::PixelFormat::R8G8B8A8_SRGB, 1, 1, pFontImage);

	Flint::ImageSamplerSpecification specification = {};
	specification.mBorderColor = Flint::BorderColor::OpaqueWhiteFLOAT;
	specification.mAddressModeU = Flint::AddressMode::ClampToEdge;
	specification.mAddressModeV = Flint::AddressMode::ClampToEdge;
	specification.mAddressModeW = Flint::AddressMode::ClampToEdge;

	pTextImageSampler = pSceneState->pDevice->CreateImageSampler(specification);

	pResourceMap = pPipeline->CreateResourceMap();
	pResourceMap->SetResource("fontSampler", pTextImageSampler, pTextImage);
}

void ImGUI::UpdateBuffers()
{
	ImDrawData* pDrawData = ImGui::GetDrawData();

	UI64 vertexSize = pDrawData->TotalVtxCount * sizeof(ImDrawVert), indexSize = pDrawData->TotalIdxCount * sizeof(ImDrawIdx);
	if ((vertexSize == 0) || (indexSize == 0))
		return;

	std::shared_ptr<Flint::Buffer> pVertexBuffer = nullptr;
	std::shared_ptr<Flint::Buffer> pIndexBuffer = nullptr;

	ImDrawVert* pVertexData = nullptr;
	ImDrawIdx* pIndexData = nullptr;

	std::shared_ptr<Flint::GeometryStore> pGeometryStore = pSceneState->pGeometryStores["ImGUI"];

	if (pGeometryStore->GetVertexCount() != pDrawData->TotalVtxCount)
	{
		pVertexBuffer = pSceneState->pDevice->CreateBuffer(Flint::BufferType::Staging, vertexSize);
		pVertexData = static_cast<ImDrawVert*>(pVertexBuffer->MapMemory(vertexSize));
	}
	else
		pVertexData = static_cast<ImDrawVert*>(pGeometryStore->MapVertexBuffer());

	if (pGeometryStore->GetIndexCount() < pDrawData->TotalIdxCount)
	{
		pIndexBuffer = pSceneState->pDevice->CreateBuffer(Flint::BufferType::Staging, indexSize);
		pIndexData = static_cast<ImDrawIdx*>(pIndexBuffer->MapMemory(indexSize));
	}
	else
		pIndexData = static_cast<ImDrawIdx*>(pGeometryStore->MapIndexBuffer());

	for (I32 i = 0; i < pDrawData->CmdListsCount; i++) {
		const ImDrawList* pCommandList = pDrawData->CmdLists[i];

		std::copy(pCommandList->VtxBuffer.Data, pCommandList->VtxBuffer.Data + pCommandList->VtxBuffer.Size, pVertexData);
		std::copy(pCommandList->IdxBuffer.Data, pCommandList->IdxBuffer.Data + pCommandList->IdxBuffer.Size, pIndexData);

		pVertexData += pCommandList->VtxBuffer.Size;
		pIndexData += pCommandList->IdxBuffer.Size;
	}

	if (pVertexBuffer)
		pVertexBuffer->UnmapMemory();

	if (pIndexBuffer)
		pIndexBuffer->UnmapMemory();

	pGeometryStore->SetData(pVertexBuffer, pIndexBuffer);

	if (pVertexBuffer)
		pVertexBuffer->Terminate();
	else
		pGeometryStore->UnmapVertexBuffer();

	if (pIndexBuffer)
		pIndexBuffer->Terminate();
	else
		pGeometryStore->UnmapIndexBuffer();
}