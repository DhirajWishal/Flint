// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ImGuiAdapter.hpp"
#include "GraphicsCore/ScreenBoundRenderTarget.hpp"
#include "GraphicsCore/ResourcePackager.hpp"
#include "Graphics/ShaderCompiler.hpp"

#include <optick.h>

constexpr Flint::uint64 ElementCount = 2500;

namespace Flint
{
	ImGuiAdapter::ImGuiAdapter()
		: pDynamicStateContainer(std::make_shared<DynamicStateContainer>())
	{
	}

	void ImGuiAdapter::Initialize(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget)
	{
		OPTICK_EVENT();

		this->pDevice = pDevice;
		this->pRenderTarget = pRenderTarget;

		if (!std::filesystem::exists(NormalizePath("Flint\\Shaders\\ImGui.vert.fsc")))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path(NormalizePath("Shaders\\ImGui\\UI.vert")), ShaderCodeType::GLSL, ShaderType::Vertex);
			pVertexShader = shaderCompiler.CreateShader(pDevice);
			pVertexShader->CreateCache(NormalizePath("Flint\\Shaders\\ImGui.vert.fsc"));
		}
		else
			pVertexShader = pDevice->CreateShader(ShaderType::Vertex, std::filesystem::path(NormalizePath("Flint\\Shaders\\ImGui.vert.fsc")));

		if (!std::filesystem::exists(NormalizePath("Flint\\Shaders\\ImGui.frag.fsc")))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path(NormalizePath("Shaders\\ImGui\\UI.frag")), ShaderCodeType::GLSL, ShaderType::Fragment);
			pFragmentShader = shaderCompiler.CreateShader(pDevice);
			pFragmentShader->CreateCache(NormalizePath("Flint\\Shaders\\ImGui.frag.fsc"));
		}
		else
			pFragmentShader = pDevice->CreateShader(ShaderType::Fragment, std::filesystem::path(NormalizePath("Flint\\Shaders\\ImGui.frag.fsc")));

		SetupImGui();
		SetupGeometryStore();
		SetupPipeline(pRenderTarget);
		SetupImage();

		pVertexShader->Terminate();
		pFragmentShader->Terminate();
	}

	void ImGuiAdapter::Initialize(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget)
	{
		OPTICK_EVENT();

		this->pDevice = pDevice;
		this->pRenderTarget = pRenderTarget;

		if (!std::filesystem::exists(NormalizePath("Flint\\Shaders\\ImGui.vert.fsc")))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path(NormalizePath("Shaders\\ImGui\\UI.vert")), ShaderCodeType::GLSL, ShaderType::Vertex);
			pVertexShader = shaderCompiler.CreateShader(pDevice);
			pVertexShader->CreateCache(NormalizePath("Flint\\Shaders\\ImGui.vert.fsc"));
		}
		else
			pVertexShader = pDevice->CreateShader(ShaderType::Vertex, std::filesystem::path(NormalizePath("Flint\\Shaders\\ImGui.vert.fsc")));

		if (!std::filesystem::exists(NormalizePath("Flint\\Shaders\\ImGui.frag.fsc")))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path(NormalizePath("Shaders\\ImGui\\UI.frag")), ShaderCodeType::GLSL, ShaderType::Fragment);
			pFragmentShader = shaderCompiler.CreateShader(pDevice);
			pFragmentShader->CreateCache(NormalizePath("Flint\\Shaders\\ImGui.frag.fsc"));
		}
		else
			pFragmentShader = pDevice->CreateShader(ShaderType::Fragment, std::filesystem::path(NormalizePath("Flint\\Shaders\\ImGui.frag.fsc")));

		SetupImGui();
		SetupGeometryStore();
		SetupPipeline(pRenderTarget);
		SetupImage();

		pVertexShader->Terminate();
		pFragmentShader->Terminate();
	}

	void ImGuiAdapter::Render(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const uint32 index)
	{
		OPTICK_EVENT();

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
			pCommandBuffer->BindGeometryStore(pGeometryStore.get());
			pCommandBuffer->BindGraphicsPipeline(pPipeline.get());

			uint64 vertexOffset = 0, indexOffset = 0;
			for (int32 i = 0; i < pDrawData->CmdListsCount; i++)
			{
				const auto pCommandList = pDrawData->CmdLists[i];

				for (int32 j = 0; j < pCommandList->CmdBuffer.Size; j++)
				{
					const auto& pCommand = pCommandList->CmdBuffer[j];

					pDynamicStateContainer->SetScissor(
						FBox2D(static_cast<uint32>(pCommand.ClipRect.z - pCommand.ClipRect.x), static_cast<uint32>(pCommand.ClipRect.w - pCommand.ClipRect.y)),
						FBox2D(std::max(static_cast<int32>(pCommand.ClipRect.x), 0), std::max(static_cast<int32>(pCommand.ClipRect.y), 0)));

					// Handle the texture.
					if (pCommand.TextureId != nullptr)
					{
						const ImGuiTextureContainer* pContainer = static_cast<ImGuiTextureContainer*>(pCommand.TextureId);
						pCommandBuffer->BindResourcePackage(pPipeline.get(), pContainer->pResourcePackage.get());
					}
					else
						pCommandBuffer->BindResourcePackage(pPipeline.get(), pResourcePacks[index].get());

					pCommandBuffer->BindDynamicStates(pPipeline.get(), pDynamicStateContainer.get());
					pCommandBuffer->IssueDrawCall(WireFrame("", vertexOffset, 0, indexOffset, pCommand.ElemCount));

					indexOffset += pCommand.ElemCount;
				}

				vertexOffset += pCommandList->VtxBuffer.Size;
			}
		}
	}

	void ImGuiAdapter::SetupPipeline(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget)
	{
		OPTICK_EVENT();

		GraphicsPipelineSpecification specification = {};
		specification.mRasterizationSamples = MultiSampleCount::One;
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

		specification.mVertexInputAttributeMap[0] = pVertexShader->GetInputAttributes();

		pPipeline = pDevice->CreateGraphicsPipeline("ImGUI", pRenderTarget, pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	}

	void ImGuiAdapter::SetupPipeline(const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget)
	{
		OPTICK_EVENT();

		GraphicsPipelineSpecification specification = {};
		//specification.mRasterizationSamples = pDevice->GetSupportedMultiSampleCount();
		specification.mRasterizationSamples = MultiSampleCount::One;
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

		specification.mVertexInputAttributeMap[0] = pVertexShader->GetInputAttributes();

		pPipeline = pDevice->CreateGraphicsPipeline("ImGUI", pRenderTarget, pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
	}

	void ImGuiAdapter::Terminate()
	{
		pPipeline->Terminate();

		pGeometryStore->UnmapVertexBuffer();
		pGeometryStore->UnmapIndexBuffer();
		pGeometryStore->Terminate();

		pFontImage->Terminate();
		pFontSampler->Terminate();
	}

	void ImGuiAdapter::SetupImGui()
	{
		OPTICK_EVENT();

		ImGui::CreateContext();
		ImGuiStyle& style = ImGui::GetStyle();

		style.Colors[ImGuiCol_TitleBg] = ImVec4(CreateColor256(34), CreateColor256(40), CreateColor256(49), 0.75f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(CreateColor256(34), CreateColor256(40), CreateColor256(49), 0.25f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(CreateColor256(34), CreateColor256(40), CreateColor256(49), 0.25f);

		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(CreateColor256(57), CreateColor256(62), CreateColor256(70), 0.8f);
		style.Colors[ImGuiCol_Header] = ImVec4(CreateColor256(57), CreateColor256(62), CreateColor256(70), 0.6f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(CreateColor256(57), CreateColor256(62), CreateColor256(70), 0.25f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(CreateColor256(57), CreateColor256(62), CreateColor256(70), 0.25f);

		style.Colors[ImGuiCol_TabHovered] = ImVec4(CreateColor256(0), CreateColor256(173), CreateColor256(181), 1.0f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(CreateColor256(0), CreateColor256(173), CreateColor256(181), 0.5f);

		style.ChildRounding = 6.0f;
		style.FrameRounding = 3.0f;
		style.PopupRounding = 3.0f;
		style.TabRounding = 3.0f;
		style.WindowRounding = 3.0f;

		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(static_cast<float>(pRenderTarget->GetExtent().mWidth), static_cast<float>(pRenderTarget->GetExtent().mHeight));
		io.DisplayFramebufferScale = ImVec2(16.0f, 9.0f);

		io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/simvoni-font/Simvoni-d9vV6.otf").string().c_str(), 12.0f);
		io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/simvoni-font/Simvoni-d9vV6.otf").string().c_str(), 8.0f);
		io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/simvoni-font/Simvoni-d9vV6.otf").string().c_str(), 10.0f);
		io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/simvoni-font/Simvoni-d9vV6.otf").string().c_str(), 14.0f);
		io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/simvoni-font/Simvoni-d9vV6.otf").string().c_str(), 16.0f);

		io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/azonix-font/Azonix-1VB0.otf").string().c_str(), 10.0f);
		io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/a-atomic-md-font/AtomicMd-OVJ4A.otf").string().c_str(), 10.0f);
		io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/josefin-sans-font/JosefinSansRegular-x3LYV.ttf").string().c_str(), 12.0f);
		io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/salma-alfasans-font/SalmaalfasansLight-d9MJx.otf").string().c_str(), 12.0f);
		io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/swansea-font/Swansea-q3pd.ttf").string().c_str(), 6.0f);
		io.Fonts->AddFontFromFileTTF(NormalizePath("Fonts/rawengulk-font/RawengulkBold-r8o9.otf").string().c_str(), 13.0f);

		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	}

	void ImGuiAdapter::SetupGeometryStore()
	{
		OPTICK_EVENT();

		pGeometryStore = pDevice->CreateGeometryStore(pVertexShader->GetInputAttributes(), sizeof(uint16), BufferMemoryProfile::TransferFriendly);
	}

	void ImGuiAdapter::SetupImage()
	{
		OPTICK_EVENT();

		ImGuiIO& imGuiIO = ImGui::GetIO();

		unsigned char* pFontImageData = nullptr;
		int width = 0, height = 0, bitsPerPixel = 0;

		imGuiIO.Fonts->GetTexDataAsRGBA32(&pFontImageData, &width, &height, &bitsPerPixel);
		for (uint64 i = 0; i < pRenderTarget->GetBufferCount(); i++)
		{
			pFontImage = pDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Graphics, FBox3D(width, height, 1), PixelFormat::R8G8B8A8_SRGB, 1, 1, pFontImageData);

			ImageSamplerSpecification specification = {};
			specification.mBorderColor = BorderColor::OpaqueWhiteFLOAT;
			specification.mAddressModeU = AddressMode::ClampToEdge;
			specification.mAddressModeV = AddressMode::ClampToEdge;
			specification.mAddressModeW = AddressMode::ClampToEdge;

			pFontSampler = pDevice->CreateImageSampler(specification);

			auto pResourcePack = pPipeline->CreateResourcePackage(0);
			pResourcePack->BindResource(0, pFontImage, pFontImage->CreateImageView(0, pFontImage->GetLayerCount(), 0, pFontImage->GetMipLevels(), ImageUsage::Graphics), pFontSampler);
			pResourcePacks.push_back(pResourcePack);
		}
	}

	void ImGuiAdapter::UpdateGeometryStore()
	{
		OPTICK_EVENT();

		ImDrawData* pDrawData = ImGui::GetDrawData();

		if (!pDrawData)
			return;

		const uint64 vertexSize = GetNewVertexBufferSize(pDrawData->TotalVtxCount * sizeof(ImDrawVert)), indexSize = GetNewIndexBufferSize(pDrawData->TotalIdxCount * sizeof(ImDrawIdx));
		if ((vertexSize == 0) || (indexSize == 0))
			return;

		std::shared_ptr<Buffer> pVertexBuffer = nullptr;
		std::shared_ptr<Buffer> pIndexBuffer = nullptr;

		bool bShouldWaitIdle = false;
		if (pGeometryStore->GetVertexCount() < pDrawData->TotalVtxCount || pDrawData->TotalVtxCount < (pGeometryStore->GetVertexCount() - ElementCount))
		{
			pGeometryStore->UnmapVertexBuffer();
			pVertexBuffer = pDevice->CreateBuffer(BufferType::Staging, vertexSize);
			pVertexData = static_cast<ImDrawVert*>(pVertexBuffer->MapMemory(vertexSize));

			bShouldWaitIdle = true;
		}

		if (pGeometryStore->GetIndexCount() < pDrawData->TotalIdxCount || pDrawData->TotalIdxCount < (pGeometryStore->GetIndexCount() - ElementCount))
		{
			pGeometryStore->UnmapIndexBuffer();
			pIndexBuffer = pDevice->CreateBuffer(BufferType::Staging, indexSize);
			pIndexData = static_cast<ImDrawIdx*>(pIndexBuffer->MapMemory(indexSize));

			bShouldWaitIdle = true;
		}

		auto pCopyVertexPointer = pVertexData;
		auto pCopyIndexPointer = pIndexData;
		for (int32 i = 0; i < pDrawData->CmdListsCount; i++) {
			const ImDrawList* pCommandList = pDrawData->CmdLists[i];

			std::copy_n(pCommandList->VtxBuffer.Data, pCommandList->VtxBuffer.Size, pCopyVertexPointer);
			std::copy_n(pCommandList->IdxBuffer.Data, pCommandList->IdxBuffer.Size, pCopyIndexPointer);

			pCopyVertexPointer += pCommandList->VtxBuffer.Size;
			pCopyIndexPointer += pCommandList->IdxBuffer.Size;
		}

		if (pVertexBuffer)
			pVertexBuffer->UnmapMemory();

		if (pIndexBuffer)
			pIndexBuffer->UnmapMemory();

		if (bShouldWaitIdle)
			pDevice->WaitForQueue();

		pGeometryStore->SetData(pVertexBuffer.get(), pIndexBuffer.get());

		if (pVertexBuffer)
		{
			pVertexBuffer->Terminate();
			pVertexData = static_cast<ImDrawVert*>(pGeometryStore->MapVertexBuffer());
		}

		if (pIndexBuffer)
		{
			pIndexBuffer->Terminate();
			pIndexData = static_cast<ImDrawIdx*>(pGeometryStore->MapIndexBuffer());
		}
	}

	uint64 ImGuiAdapter::GetNewVertexBufferSize(const uint64 newSize) const
	{
		constexpr uint64 VertexFactor = ElementCount * sizeof(ImDrawVert);
		const auto count = (newSize / VertexFactor) + 1;
		return count * VertexFactor;
	}

	uint64 ImGuiAdapter::GetNewIndexBufferSize(const uint64 newSize) const
	{
		constexpr uint64 IndexFactor = ElementCount * sizeof(ImDrawIdx);
		const auto count = (newSize / IndexFactor) + 1;
		return count * IndexFactor;
	}
}
