// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Client/GraphicsBase.hpp"
#include "GraphicsCore/Instance.hpp"
#include "Engine/ShaderCompiler.hpp"

namespace Flint
{
	void GraphicsBase::CreateInstance(bool enablevalidation)
	{
		pInstance = Flint::CreateInstance(enablevalidation);
	}

	void GraphicsBase::DestroyInstance()
	{
		if (pInstance)
			pInstance->Terminate();
	}

	void GraphicsBase::SetInstance(const std::shared_ptr<Instance>& pInstance)
	{
		DestroyInstance();
		this->pInstance = pInstance;
	}

	void GraphicsBase::CreateNewDevice(const DeviceFlags flags)
	{
		auto pDevice = pInstance->CreateDevice(flags);

		if (!pDefaultDevice)
			pDefaultDevice = pDevice;

		pDevices.push_back(pDevice);
	}

	void GraphicsBase::SetDevices(const std::vector<std::shared_ptr<Device>>& pDevices)
	{
		DestroyAllDevices();
		this->pDevices = pDevices;

		if (!pDevices.empty())
			pDefaultDevice = pDevices.front();
	}

	void GraphicsBase::SetDefaultDevice(const UI32 index)
	{
		pDefaultDevice = pDevices[index];
	}

	const std::shared_ptr<Device> GraphicsBase::GetDefaultDevice() const
	{
		return pDefaultDevice;
	}

	void GraphicsBase::DestroyDevice(const UI32 index)
	{
		pDevices[index]->Terminate();
		pDevices.erase(pDevices.begin() + index);
	}

	void GraphicsBase::DestroyAllDevices()
	{
		for (auto& pDevice : pDevices)
			pDevice->Terminate();

		pDevices.clear();
	}

	std::shared_ptr<ScreenBoundRenderTarget> GraphicsBase::CreateDefaultScreenBoundRenderTarget(const FBox2D& extent, const std::string& windowTitle)
	{
		std::shared_ptr<Display> pDisplay = pInstance->CreateDisplay(extent, windowTitle);
		const auto sampleCount = pDefaultDevice->GetSupportedMultiSampleCount();
		const FBox3D imageExtent = FBox3D(extent.mWidth, extent.mHeight, 1);

		std::vector<RenderTargetAttachment> attachments(2);
		attachments[0] = RenderTargetAttachment(
			pDefaultDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Color, imageExtent, pDisplay->GetBestSwapChainFormat(pDefaultDevice), 1, 1, nullptr, sampleCount),
			FColor4D(CreateColor256(32.0f), CreateColor256(32.0f), CreateColor256(32.0f), 1.0f));

		attachments[1] = RenderTargetAttachment(
			pDefaultDevice->CreateImage(ImageType::TwoDimension, ImageUsage::Depth, imageExtent, PixelFormat::D24_UNORMAL_S8_UINT, 1, 1, nullptr, sampleCount),
			DepthClearValues(1.0f, 0));

		std::shared_ptr<ScreenBoundRenderTarget> pRenderTarget = pDefaultDevice->CreateScreenBoundRenderTarget(pDisplay, extent, pDisplay->FindBestBufferCount(pDefaultDevice), attachments, SwapChainPresentMode::MailBox);
		pScreenBoundRenderTargets["Default"] = pRenderTarget;
		
		return pRenderTarget;
	}

	void GraphicsBase::SetDefaultScreenBoundRenderTarget(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget)
	{
		pScreenBoundRenderTargets["Default"] = pRenderTarget;
	}

	const std::shared_ptr<ScreenBoundRenderTarget> GraphicsBase::GetScreenBoundRenderTarget(const std::string& name) const
	{
		if (pScreenBoundRenderTargets.find(name) == pScreenBoundRenderTargets.end())
			return nullptr;

		return pScreenBoundRenderTargets.at(name);
	}

	const std::shared_ptr<ScreenBoundRenderTarget> GraphicsBase::GetDefaultScreenBoundRenderTarget() const
	{
		return GetScreenBoundRenderTarget("Default");
	}

	std::shared_ptr<Shader> GraphicsBase::CreateShaderFromBinary(const std::filesystem::path& asset, const ShaderType type)
	{
		return pDefaultDevice->CreateShader(type, asset);
	}

	std::shared_ptr<Shader> GraphicsBase::CompileAndCreateShader(const std::filesystem::path& asset, const ShaderCodeType codeType, const ShaderType type)
	{
		ShaderCompiler compiler(asset, codeType, type);
		return pDefaultDevice->CreateShader(type, compiler.GetShaderCode());
	}

	std::shared_ptr<GraphicsPipeline> GraphicsBase::GetGraphicsPipeline(const std::string& name) const
	{
		if (pGraphicsPipelines.find(name) == pGraphicsPipelines.end())
			return nullptr;

		return pGraphicsPipelines.at(name);
	}

	std::shared_ptr<GraphicsPipeline> GraphicsBase::CreateGraphicsPipeline(const std::string& name, const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget, const std::shared_ptr<Shader>& pVertexShader, const std::shared_ptr<Shader>& pFragmentShader, const GraphicsPipelineSpecification& specification)
	{
		std::shared_ptr<GraphicsPipeline> pPipeline = pDefaultDevice->CreateGraphicsPipeline(name, pScreenBoundRenderTarget, pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
		pGraphicsPipelines[name] = pPipeline;
		
		return pPipeline;
	}
}