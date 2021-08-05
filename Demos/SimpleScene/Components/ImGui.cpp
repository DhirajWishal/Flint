// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ImGui.hpp"

ImGUI::ImGUI(glm::vec3 position, SceneState* pSceneState)
	: GameObject(position, pSceneState)
{
	PrepareGeometryStore();
	PreparePipeline();
	PrepareImage();
}

ImGUI::~ImGUI()
{

}

void ImGUI::OnUpdate(UI64 delta)
{
}

void ImGUI::PrepareGeometryStore()
{
	pSceneState->pGeometryStores["ImGUI"] = pSceneState->pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(UI16));
}

void ImGUI::PreparePipeline()
{
	Flint::GraphicsPipelineSpecification specification = {};

	pSceneState->pGraphicsPipelines["ImGUI"] = pSceneState->pDevice->CreateGraphicsPipeline("ImGUI", pSceneState->pScreenBoundRenderTargets["Default"], pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	pSceneState->pScreenBoundRenderTargets["Default"]->SubmitPipeline(pSceneState->pGeometryStores["ImGUI"], pSceneState->pGraphicsPipelines["ImGUI"]);
}

void ImGUI::PrepareImage()
{
	ImGuiIO& imGuiIO = ImGui::GetIO();

	unsigned char* pFontImage = nullptr;
	int width = 0, height = 0, bitsPerPixel = 0;

	imGuiIO.Fonts->GetTexDataAsRGBA32(&pFontImage, &width, &height, &bitsPerPixel);
	pTextImage = pSceneState->pDevice->CreateImage(Flint::ImageType::DIMENSIONS_2, Flint::ImageUsage::GRAPHICS, Flint::FBox3D(width, height, 1), Flint::PixelFormat::R8G8B8A8_SRGB, 1, 1, pFontImage);
}

void ImGUI::UpdateBuffers()
{
	ImDrawData* pDrawData = ImGui::GetDrawData();

	UI64 vertexSize = pDrawData->TotalVtxCount * sizeof(ImDrawVert), indexSize = pDrawData->TotalIdxCount * sizeof(ImDrawIdx);
	if ((vertexSize == 0) || (indexSize == 0))
		return;

	std::shared_ptr<Flint::Buffer> pVertexBuffer = nullptr;
	std::shared_ptr<Flint::Buffer> pIndexBuffer = nullptr;

	if (pSceneState->pGeometryStores["ImGUI"]->GetVertexCount() != pDrawData->TotalVtxCount)
		pVertexBuffer = pSceneState->pDevice->CreateBuffer(Flint::BufferType::STAGGING, vertexSize);

	if (pSceneState->pGeometryStores["ImGUI"]->GetIndexCount() != pDrawData->TotalIdxCount)
		pIndexBuffer = pSceneState->pDevice->CreateBuffer(Flint::BufferType::STAGGING, indexSize);

	ImDrawVert* pVertexData = nullptr;
	ImDrawIdx* pIndexData = nullptr;

	if (pVertexBuffer)
		pVertexData = static_cast<ImDrawVert*>(pVertexBuffer->MapMemory(vertexSize));

	if (pIndexBuffer)
		pIndexData = static_cast<ImDrawIdx*>(pIndexBuffer->MapMemory(indexSize));

	for (UI32 i = 0; i < pDrawData->CmdListsCount; i++) {
		const ImDrawList* cmd_list = pDrawData->CmdLists[i];

		if (pVertexData)
			std::copy(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Data + cmd_list->VtxBuffer.Size, pVertexData);

		if (pIndexData)
			std::copy(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Data + cmd_list->IdxBuffer.Size, pIndexData);

		pVertexData += cmd_list->VtxBuffer.Size;
		pIndexData += cmd_list->IdxBuffer.Size;
	}

	if (pVertexBuffer)
		pVertexBuffer->UnmapMemory();

	if (pIndexBuffer)
		pIndexBuffer->UnmapMemory();

	pSceneState->pGeometryStores["ImGUI"]->SetData(pVertexBuffer, pIndexBuffer);
}
