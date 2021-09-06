// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ImGuiAdapter.hpp"
#include "GraphicsCore/ScreenBoundRenderTarget.hpp"
#include "GraphicsCore/ResourcePackager.hpp"
#include "Engine/ShaderCompiler.hpp"

#include <iostream>

namespace Flint
{
	ImGuiAdapter::ImGuiAdapter()
		: pDynamicStateContainer(std::make_shared<DynamicStateContainer>())
	{
	}

	void ImGuiAdapter::Initialize(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget)
	{
		this->pDevice = pDevice;
		this->pRenderTarget = pRenderTarget;

		ShaderCompiler vertexShaderCompiler(std::filesystem::path("E:\\Flint\\Editor\\Shaders\\ImGui\\UI.vert"), ShaderCodeType::GLSL, ShaderType::Vertex);
		ShaderCompiler fragmentShaderCompiler(std::filesystem::path("E:\\Flint\\Editor\\Shaders\\ImGui\\UI.frag"), ShaderCodeType::GLSL, ShaderType::Fragment);

		pVertexShader = pDevice->CreateShader(ShaderType::Vertex, vertexShaderCompiler.GetShaderCode());
		pFragmentShader = pDevice->CreateShader(ShaderType::Fragment, fragmentShaderCompiler.GetShaderCode());

		SetupImGui();
		SetupGeometryStore();
		SetupPipeline();
		SetupImage();
	}

	void ImGuiAdapter::Render(const std::shared_ptr<CommandBuffer>& pCommandBuffer)
	{
		ImGui::Render();
		UpdateGeometryStore();

		ImGuiIO& io = ImGui::GetIO();
		ImDrawData* pDrawData = ImGui::GetDrawData();

		if (!pDrawData)
			return;

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		pushConstants.mScale = glm::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
		pushConstants.mTranslate = glm::vec2(-1.0f);

		pDynamicStateContainer->SetViewPort(FExtent2D<float>(io.DisplaySize.x, io.DisplaySize.y), FExtent2D<float>(0.0f, 1.0f), FExtent2D<float>());
		pDynamicStateContainer->SetConstantData(ShaderType::Vertex, &pushConstants, sizeof(PushConstants));

		if (pDrawData->CmdListsCount)
		{
			pCommandBuffer->BindGeometryStore(pGeometryStore);
			pCommandBuffer->BindGraphicsPipeline(pPipeline);

			UI64 vertexOffset = 0, indexOffset = 0;
			for (I32 i = 0; i < pDrawData->CmdListsCount; i++)
			{
				auto pCommandList = pDrawData->CmdLists[i];

				for (I32 j = 0; j < pCommandList->CmdBuffer.Size; j++)
				{
					auto& pCommand = pCommandList->CmdBuffer[j];

					pDynamicStateContainer->SetScissor(
						FBox2D(static_cast<UI32>(pCommand.ClipRect.z - pCommand.ClipRect.x), static_cast<UI32>(pCommand.ClipRect.w - pCommand.ClipRect.y)),
						FBox2D(std::max(static_cast<I32>(pCommand.ClipRect.x), 0), std::max(static_cast<I32>(pCommand.ClipRect.y), 0)));

					// Handle the texture.
					if (pCommand.TextureId != nullptr)
					{
						const ImGuiTextureContainer* pContainer = reinterpret_cast<ImGuiTextureContainer*>(pCommand.TextureId);
						pCommandBuffer->BindResourcePackages(pPipeline, { pContainer->pResourcePackage });
					}
					else
						pCommandBuffer->BindResourcePackages(pPipeline, { pResourcePack });

					pCommandBuffer->BindDynamicStates(pPipeline, pDynamicStateContainer);
					pCommandBuffer->IssueDrawCall(WireFrame("", vertexOffset, 0, indexOffset, pCommand.ElemCount));

					indexOffset += pCommand.ElemCount;
				}

				vertexOffset += pCommandList->VtxBuffer.Size;
			}
		}
	}

	void ImGuiAdapter::Terminate()
	{
		pPipeline->Terminate();
		pResourcePackager->Terminate();

		pVertexShader->Terminate();
		pFragmentShader->Terminate();

		pGeometryStore->Terminate();

		pFontImage->Terminate();
		pFontSampler->Terminate();
	}

	void ImGuiAdapter::SetupImGui()
	{
		ImGui::CreateContext();
		ImGuiStyle& style = ImGui::GetStyle();

		style.Colors[ImGuiCol_TitleBg]					= ImVec4(CreateColor256(34), CreateColor256(40), CreateColor256(49), 0.75f);	
		style.Colors[ImGuiCol_WindowBg]					= ImVec4(CreateColor256(34), CreateColor256(40), CreateColor256(49), 0.25f);
		style.Colors[ImGuiCol_MenuBarBg]				= ImVec4(CreateColor256(34), CreateColor256(40), CreateColor256(49), 0.1f);

		style.Colors[ImGuiCol_TitleBgActive]			= ImVec4(CreateColor256(57), CreateColor256(62), CreateColor256(70), 0.8f);	
		style.Colors[ImGuiCol_Header]					= ImVec4(CreateColor256(57), CreateColor256(62), CreateColor256(70), 0.6f);
		style.Colors[ImGuiCol_TabActive]				= ImVec4(CreateColor256(57), CreateColor256(62), CreateColor256(70), 0.25f);
		style.Colors[ImGuiCol_TabUnfocusedActive]		= ImVec4(CreateColor256(57), CreateColor256(62), CreateColor256(70), 0.25f);

		style.Colors[ImGuiCol_TabHovered]				= ImVec4(CreateColor256(0), CreateColor256(173), CreateColor256(181), 1.0f);
		style.Colors[ImGuiCol_HeaderHovered]			= ImVec4(CreateColor256(0), CreateColor256(173), CreateColor256(181), 0.5f);

		style.ChildRounding		= 6.0f;
		style.FrameRounding		= 3.0f;
		style.PopupRounding		= 3.0f;
		style.TabRounding		= 3.0f;
		style.WindowRounding	= 3.0f;

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(static_cast<float>(pRenderTarget->GetExtent().mWidth), static_cast<float>(pRenderTarget->GetExtent().mHeight));
		io.DisplayFramebufferScale = ImVec2(16.0f, 9.0f);

		io.Fonts->AddFontFromFileTTF("Fonts/simvoni-font/Simvoni-d9vV6.otf", 12.0f);
		io.Fonts->AddFontFromFileTTF("Fonts/simvoni-font/Simvoni-d9vV6.otf", 8.0f);
		io.Fonts->AddFontFromFileTTF("Fonts/simvoni-font/Simvoni-d9vV6.otf", 10.0f);
		io.Fonts->AddFontFromFileTTF("Fonts/simvoni-font/Simvoni-d9vV6.otf", 14.0f);
		io.Fonts->AddFontFromFileTTF("Fonts/simvoni-font/Simvoni-d9vV6.otf", 16.0f);

		io.Fonts->AddFontFromFileTTF("Fonts/azonix-font/Azonix-1VB0.otf", 10.0f);
		io.Fonts->AddFontFromFileTTF("Fonts/a-atomic-md-font/AtomicMd-OVJ4A.otf", 10.0f);
		io.Fonts->AddFontFromFileTTF("Fonts/josefin-sans-font/JosefinSansRegular-x3LYV.ttf", 12.0f);
		io.Fonts->AddFontFromFileTTF("Fonts/salma-alfasans-font/SalmaalfasansLight-d9MJx.otf", 12.0f);
		io.Fonts->AddFontFromFileTTF("Fonts/swansea-font/Swansea-q3pd.ttf", 6.0f);
		io.Fonts->AddFontFromFileTTF("Fonts/rawengulk-font/RawengulkBold-r8o9.otf", 13.0f);

		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	}

	void ImGuiAdapter::SetupGeometryStore()
	{
		pGeometryStore = pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(UI16), BufferMemoryProfile::TransferFriendly);
	}

	void ImGuiAdapter::SetupPipeline()
	{
		GraphicsPipelineSpecification specification = {};
		specification.mRasterizationSamples = pDevice->GetSupportedMultiSampleCount();
		//specification.mRasterizationSamples = MultiSampleCount::One;
		specification.mDynamicStateFlags = DynamicStateFlags::ViewPort | DynamicStateFlags::Scissor;
		specification.mCullMode = CullMode::None;
		specification.mColorBlendAttachments[0].mEnableBlend = true;
		specification.mColorBlendAttachments[0].mSrcBlendFactor = ColorBlendFactor::SourceAlpha;
		specification.mColorBlendAttachments[0].mDstBlendFactor = ColorBlendFactor::OneMinusSourceAlpha;
		specification.mColorBlendAttachments[0].mSrcAlphaBlendFactor = ColorBlendFactor::OneMinusSourceAlpha;
		specification.mColorBlendAttachments[0].mDstAlphaBlendFactor = ColorBlendFactor::Zero;
		specification.mDepthCompareLogic = DepthCompareLogic::LessOrEqual;

		specification.mColorBlendConstants[0] = 0.0f;
		specification.mColorBlendConstants[1] = 0.0f;
		specification.mColorBlendConstants[2] = 0.0f;
		specification.mColorBlendConstants[3] = 0.0f;

		specification.bEnableSampleShading = false;
		specification.mMinSampleShading = 0.0f;

		pPipeline = pDevice->CreateGraphicsPipeline("ImGUI", pRenderTarget, pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	}

	void ImGuiAdapter::SetupImage()
	{
		ImGuiIO& imGuiIO = ImGui::GetIO();

		unsigned char* pFontImageData = nullptr;
		int width = 0, height = 0, bitsPerPixel = 0;

		imGuiIO.Fonts->GetTexDataAsRGBA32(&pFontImageData, &width, &height, &bitsPerPixel);
		pFontImage = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Graphics, FBox3D(width, height, 1), PixelFormat::R8G8B8A8_SRGB, 1, 1, pFontImageData);

		ImageSamplerSpecification specification = {};
		specification.mBorderColor = BorderColor::OpaqueWhiteFLOAT;
		specification.mAddressModeU = AddressMode::ClampToEdge;
		specification.mAddressModeV = AddressMode::ClampToEdge;
		specification.mAddressModeW = AddressMode::ClampToEdge;

		pFontSampler = pDevice->CreateImageSampler(specification);

		pResourcePackager = pPipeline->CreateResourcePackagers()[0];
		pResourcePack = pResourcePackager->CreatePackage();
		pResourcePack->BindResource(0, pFontSampler, pFontImage);
	}

	void ImGuiAdapter::UpdateGeometryStore()
	{
		ImDrawData* pDrawData = ImGui::GetDrawData();

		if (!pDrawData)
			return;

		UI64 vertexSize = pDrawData->TotalVtxCount * sizeof(ImDrawVert), indexSize = pDrawData->TotalIdxCount * sizeof(ImDrawIdx);
		if ((vertexSize == 0) || (indexSize == 0))
			return;

		std::shared_ptr<Buffer> pVertexBuffer = nullptr;
		std::shared_ptr<Buffer> pIndexBuffer = nullptr;

		if (pGeometryStore->GetVertexCount() != pDrawData->TotalVtxCount)
		{
			pVertexBuffer = pDevice->CreateBuffer(BufferType::Staging, vertexSize);
			pVertexData = static_cast<ImDrawVert*>(pVertexBuffer->MapMemory(vertexSize));
		}
		else
			pVertexData = static_cast<ImDrawVert*>(pGeometryStore->MapVertexBuffer());

		if (pGeometryStore->GetIndexCount() < pDrawData->TotalIdxCount)
		{
			pIndexBuffer = pDevice->CreateBuffer(BufferType::Staging, indexSize);
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
}
