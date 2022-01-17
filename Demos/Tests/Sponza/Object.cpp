// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Object.hpp"

#include "TestBase/GraphicsScene.hpp"
#include "Graphics/Tools/ShaderCompiler.hpp"
#include "Graphics/Tools/ImageLoader.hpp"
#include "Graphics/Tools/CubeMapGenerator.hpp"
#include "GraphicsCore/Query.hpp"

#include <glm/gtx/transform.hpp>
#include <optick.h>
#include <future>

namespace Flint
{
	Object::Object(Application* pApplication, std::shared_ptr<OffScreenPass> pOffScreenPass)
		: pApplication(pApplication), pOffScreenPass(pOffScreenPass)
	{
		mAsyncStatus = pApplication->mGraphicsWorker.IssueWork([this, pApplication]() {
			pDynamicStates = std::make_shared<DynamicStateContainer>();
			pDynamicStatesLeftEye = std::make_shared<DynamicStateContainer>();
			pDynamicStatesRightEye = std::make_shared<DynamicStateContainer>();

			pUniformBuffer = pApplication->GetDevice()->CreateBuffer(BufferType::Uniform, sizeof(ModelViewProjection));

			//pApplication->GetGraphicsScene("Default")->mCamera.SetCameraRange(0.01f, 2048.0f);
			mMatrix.mModelMatrix = glm::scale(mMatrix.mModelMatrix, glm::vec3(0.005f));

			CreatePipeline();
			CreateOcclusionPipeline();
			LoadAsset();
			LoadTextures();

			return true;
			});
	}

	void Object::Initialize(Application* pApplication)
	{
		OPTICK_EVENT();

		this->pApplication = pApplication;

		mAsyncStatus = pApplication->mGraphicsWorker.IssueWork([this, pApplication]() {
			pDynamicStates = std::make_shared<DynamicStateContainer>();
			pDynamicStatesLeftEye = std::make_shared<DynamicStateContainer>();
			pDynamicStatesRightEye = std::make_shared<DynamicStateContainer>();

			pUniformBuffer = pApplication->GetDevice()->CreateBuffer(BufferType::Uniform, sizeof(ModelViewProjection));

			//pApplication->GetGraphicsScene("Default")->mCamera.SetCameraRange(0.01f, 2048.0f);
			mMatrix.mModelMatrix = glm::scale(mMatrix.mModelMatrix, glm::vec3(0.005f));

			CreatePipeline();
			CreateOcclusionPipeline();
			LoadAsset();
			LoadTextures();

			return true;
			});
	}

	void Object::Update(uint64 delta, Camera* pCamera)
	{
		OPTICK_EVENT();

		if (bFirstAsync && mAsyncStatus.valid() && mAsyncStatus.get())
			bFirstAsync = false;

		const auto mat = pCamera->GetMatrix();
		mMatrix.mViewMatrix = mat.mViewMatrix;
		mMatrix.mProjectionMatrix = mat.mProjectionMatrix;

		const auto leftEyeMatrix = glm::lookAt(pCamera->GetPosition() + glm::vec3(mEyeExpansion, 0.0f, 0.0f), pCamera->GetPosition() + pCamera->GetCameraFront(), pCamera->GetCameraUp());
		const auto rightEyeMatrix = glm::lookAt(pCamera->GetPosition() - glm::vec3(mEyeExpansion, 0.0f, 0.0f), pCamera->GetPosition() + pCamera->GetCameraFront(), pCamera->GetCameraUp());

		pDynamicStatesLeftEye->SetConstantData(Flint::ShaderType::Vertex, &leftEyeMatrix, sizeof(leftEyeMatrix));
		pDynamicStatesRightEye->SetConstantData(Flint::ShaderType::Vertex, &rightEyeMatrix, sizeof(rightEyeMatrix));

		ImGui::Begin("Settings");
		ImGui::SliderFloat("Bias", &mMatrix.mMipBias, 0, 10);
		ImGui::SliderFloat("Eye expansion", &mEyeExpansion, 0, 10);
		ImGui::End();

		CopyToBuffer(pUniformBuffer, mMatrix);
	}

	void Object::Draw(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const uint32 index)
	{
		OPTICK_EVENT();

		uint64 drawCount = 0;
		const uint64 count = mAsset.GetWireFrames().size();
		auto& wireFrames = mAsset.GetWireFrames();

		if (bStereoVision)
		{
			// Left eye
			{
				pCommandBuffer->BindGraphicsPipeline(pPipeline.get());
				pCommandBuffer->BindGeometryStore(mAsset.GetGeometryStore().get());
				pCommandBuffer->BindDynamicStates(pPipeline.get(), pDynamicStatesLeftEye.get());

				drawCount = 0;
				const auto& samples = mDrawSamples[index];
				for (uint64 i = 0; i < count; i++)
				{
					if (samples[i] == 0 && !bSkipCulling)
						continue;

					auto& wireFrame = wireFrames[i];
					auto& pPackage = pPackageSets[index][i];

					pCommandBuffer->BindResourcePackage(pPipeline.get(), pPackage.get());
					pCommandBuffer->IssueDrawCall(wireFrame);
					drawCount++;
				}
			}

			// Right eye
			{
				pCommandBuffer->BindGraphicsPipeline(pPipeline.get());
				pCommandBuffer->BindGeometryStore(mAsset.GetGeometryStore().get());
				pCommandBuffer->BindDynamicStates(pPipeline.get(), pDynamicStatesRightEye.get());

				drawCount = 0;
				const auto& samples = mDrawSamples[index];
				for (uint64 i = 0; i < count; i++)
				{
					if (samples[i] == 0 && !bSkipCulling)
						continue;

					auto& wireFrame = wireFrames[i];
					auto& pPackage = pPackageSets[index][i];

					pCommandBuffer->BindResourcePackage(pPipeline.get(), pPackage.get());
					pCommandBuffer->IssueDrawCall(wireFrame);
					drawCount++;
				}
			}
		}
		else
		{
			pCommandBuffer->BindGraphicsPipeline(pPipeline.get());
			pCommandBuffer->BindGeometryStore(mAsset.GetGeometryStore().get());
			pCommandBuffer->BindDynamicStates(pPipeline.get(), pDynamicStates.get());

			drawCount = 0;
			const auto& samples = mDrawSamples[index];
			for (uint64 i = 0; i < count; i++)
			{
				if (samples[i] == 0 && !bSkipCulling)
					continue;

				auto& wireFrame = wireFrames[i];
				auto& pPackage = pPackageSets[index][i];

				pCommandBuffer->BindResourcePackage(pPipeline.get(), pPackage.get());
				pCommandBuffer->IssueDrawCall(wireFrame);
				drawCount++;
			}
		}

		ImGui::Begin("Occlusion Culling");
		ImGui::Text("Count: %u / %u", drawCount, count);
		ImGui::Checkbox("Pause Occlusion Culling", &bShouldFreezeOcclusion);
		ImGui::Checkbox("Skip Occlusion Culling", &bSkipCulling);
		ImGui::Checkbox("Stereo vision", &bStereoVision);
		ImGui::End();
	}

	void Object::OcclusionPass(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const uint32 index)
	{
		OPTICK_EVENT();

		// Return if we need to skip culling.
		if (bSkipCulling)
			return;

		pCommandBuffer->BindGraphicsPipeline(pOcclusionPipeline.get());
		pCommandBuffer->BindGeometryStore(mAsset.GetGeometryStore().get());
		pCommandBuffer->BindDynamicStates(pOcclusionPipeline.get(), pDynamicStates.get());
		pCommandBuffer->BindResourcePackage(pOcclusionPipeline.get(), pOcclusionPackageSets[index].get());

		const auto pQuery = pOcclusionQueries[index];

		const uint64 count = mAsset.GetWireFrames().size();
		auto& wireFrames = mAsset.GetWireFrames();
		for (uint64 i = 0; i < count; i++)
		{
			auto& wireFrame = wireFrames[i];

			pCommandBuffer->BeginQuery(pQuery.get(), static_cast<uint32>(i));
			pCommandBuffer->IssueDrawCall(wireFrame);
			pCommandBuffer->EndQuery(pQuery.get(), static_cast<uint32>(i));
		}
	}

	void Object::ResetOcclusionQuery(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const uint32 index, const bool isFirstUse)
	{
		const auto pQuery = pOcclusionQueries[index];
		if (!isFirstUse && !bShouldFreezeOcclusion && !bSkipCulling)
		{
			auto& samples = mDrawSamples[index];
			pQuery->RequestQueryData(0, pQuery->GetQueryCount(), sizeof(uint64) * samples.size(), samples.data(), sizeof(uint64), QueryDataMode::UI64Result | QueryDataMode::WaitForResult);
		}

		pCommandBuffer->ResetQuery(pQuery.get(), 0, pQuery->GetQueryCount());
	}

	void Object::Terminate()
	{
		pUniformBuffer->Terminate();
		pPipeline->Terminate();
		pOcclusionPipeline->Terminate();
		pPackageSets.clear();

		mAsset.Clear();
	}

	void Object::CreatePipeline()
	{
		OPTICK_EVENT();

		const auto pDevice = pApplication->GetDevice();
		std::shared_ptr<Shader> pVertexShader = nullptr;
		std::shared_ptr<Shader> pFragmentShader = nullptr;

		if (!std::filesystem::exists(NormalizePath("Flint\\Shaders\\Object.vert.fsc")))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path(NormalizePath("Shaders\\Object\\Object.vert")), ShaderCodeType::GLSL, ShaderType::Vertex);
			pVertexShader = shaderCompiler.CreateShader(pDevice);
			pVertexShader->CreateCache(NormalizePath("Flint\\Shaders\\Object.vert.fsc"));
		}
		else
			pVertexShader = pDevice->CreateShader(ShaderType::Vertex, std::filesystem::path(NormalizePath("Flint\\Shaders\\Object.vert.fsc")));

		if (!std::filesystem::exists(NormalizePath("Flint\\Shaders\\Object.frag.fsc")))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path(NormalizePath("Shaders\\Object\\Object.frag")), ShaderCodeType::GLSL, ShaderType::Fragment);
			pFragmentShader = shaderCompiler.CreateShader(pDevice);
			pFragmentShader->CreateCache(NormalizePath("Flint\\Shaders\\Object.frag.fsc"));
		}
		else
			pFragmentShader = pDevice->CreateShader(ShaderType::Fragment, std::filesystem::path(NormalizePath("Flint\\Shaders\\Object.frag.fsc")));

		Flint::GraphicsPipelineSpecification specification = {};
		specification.mRasterizationSamples = pOffScreenPass->GetMultiSampleCount();
		specification.mDynamicStateFlags = Flint::DynamicStateFlags::ViewPort | Flint::DynamicStateFlags::Scissor;
		specification.mColorBlendAttachments.resize(2);

		specification.mVertexInputAttributeMap[0] = pVertexShader->GetInputAttributes();
		pPipeline = pDevice->CreateGraphicsPipeline("Object", pOffScreenPass->GetRenderTarget(), pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);
		pApplication->CreateGeometryStore("Object", pVertexShader->GetInputAttributes(), sizeof(uint32));

		const auto windowExtent = pOffScreenPass->GetExtent();
		const auto viewPort = Flint::FExtent2D<float>{ static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight) };
		pDynamicStates->SetViewPort(viewPort, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
		pDynamicStates->SetScissor(windowExtent, { 0, 0 });

		const auto viewPortHalf = Flint::FExtent2D<float>{ static_cast<float>(windowExtent.mWidth) / 2, static_cast<float>(windowExtent.mHeight) };
		pDynamicStatesLeftEye->SetViewPort(viewPortHalf, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
		pDynamicStatesLeftEye->SetScissor(windowExtent, { 0, 0 });

		pDynamicStatesRightEye->SetViewPort(viewPortHalf, Flint::FExtent2D<float>(0.0f, 1.0f), { viewPortHalf.mWidth, 0.0f });
		pDynamicStatesRightEye->SetScissor(windowExtent, { 0, 0 });

		pVertexShader->Terminate();
		pFragmentShader->Terminate();
	}

	void Object::CreateOcclusionPipeline()
	{
		OPTICK_EVENT();

		const auto pDevice = pApplication->GetDevice();
		std::shared_ptr<Shader> pVertexShader = nullptr;
		std::shared_ptr<Shader> pFragmentShader = nullptr;

		if (!std::filesystem::exists(NormalizePath("Flint\\Shaders\\Occlusion.vert.fsc")))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path(NormalizePath("Shaders\\Object\\Occlusion.vert")), ShaderCodeType::GLSL, ShaderType::Vertex);
			pVertexShader = shaderCompiler.CreateShader(pDevice);
			pVertexShader->CreateCache(NormalizePath("Flint\\Shaders\\Occlusion.vert.fsc"));
		}
		else
			pVertexShader = pDevice->CreateShader(ShaderType::Vertex, std::filesystem::path(NormalizePath("Flint\\Shaders\\Occlusion.vert.fsc")));

		if (!std::filesystem::exists(NormalizePath("Flint\\Shaders\\Occlusion.frag.fsc")))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path(NormalizePath("Shaders\\Object\\Occlusion.frag")), ShaderCodeType::GLSL, ShaderType::Fragment);
			pFragmentShader = shaderCompiler.CreateShader(pDevice);
			pFragmentShader->CreateCache(NormalizePath("Flint\\Shaders\\Occlusion.frag.fsc"));
		}
		else
			pFragmentShader = pDevice->CreateShader(ShaderType::Fragment, std::filesystem::path(NormalizePath("Flint\\Shaders\\Occlusion.frag.fsc")));

		Flint::GraphicsPipelineSpecification specification = {};
		specification.mRasterizationSamples = pOffScreenPass->GetMultiSampleCount();
		specification.mDynamicStateFlags = Flint::DynamicStateFlags::ViewPort | Flint::DynamicStateFlags::Scissor;
		specification.mColorBlendAttachments.resize(2);

		specification.mVertexInputAttributeMap[0] = pVertexShader->GetInputAttributes();
		pOcclusionPipeline = pDevice->CreateGraphicsPipeline("Occlusion", pOffScreenPass->GetRenderTarget(), pVertexShader, nullptr, nullptr, nullptr, pFragmentShader, specification);

		pVertexShader->Terminate();
		pFragmentShader->Terminate();
	}

	void Object::LoadAsset()
	{
		OPTICK_EVENT();

		Flint::VertexDescriptor vDescriptor = {};
		vDescriptor.mAttributes.emplace_back(Flint::VertexAttribute(sizeof(glm::vec3), Flint::InputAttributeType::Position));
		vDescriptor.mAttributes.emplace_back(Flint::VertexAttribute(sizeof(glm::vec3), Flint::InputAttributeType::Normal));
		vDescriptor.mAttributes.emplace_back(Flint::VertexAttribute(sizeof(glm::vec2), Flint::InputAttributeType::TextureCoordinatesZero));

		mAsset = Asset("E:\\Projects\\Lighter\\Assets\\2.0\\Sponza\\glTF\\Sponza.gltf", pApplication->GetGeometryStore("Object"), vDescriptor);

		mDrawSamples.reserve(pOffScreenPass->GetRenderTarget()->GetBufferCount());
		pOcclusionQueries.reserve(pOffScreenPass->GetRenderTarget()->GetBufferCount());

		const auto size = mAsset.GetWireFrames().size();
		for (uint32 i = 0; i < pOffScreenPass->GetRenderTarget()->GetBufferCount(); i++)
		{
			mDrawSamples.emplace_back(std::vector<uint64>(size));
			pOcclusionQueries.emplace_back(pApplication->GetDevice()->CreateQuery(QueryUsage::Occlusion, static_cast<uint32>(size)));
		}
	}

	std::mutex mImageLocker = {};

	void Object::LoadTextures()
	{
		OPTICK_EVENT();

		std::unordered_map<std::shared_ptr<MaterialProperties::TexturePath>, ImageLoader> loaderMap = {};

		{
			std::vector<std::future<void>> futures;
			for (auto& wireFrame : mAsset.GetWireFrames())
			{
				Material const& material = wireFrame.GetMaterial();

				for (const auto pProperty : material.GetProperties())
				{
					if (pProperty->GetType() == MaterialProperties::MaterialType::TEXTURE_PATH)
					{
						auto pTexturePath = std::static_pointer_cast<MaterialProperties::TexturePath>(pProperty);
						if (pTexturePath->mType != MaterialProperties::TextureType::Diffuse)
							continue;

						futures.emplace_back(std::async(std::launch::async, [](const std::shared_ptr<MaterialProperties::TexturePath>& pTexturePath,
							std::unordered_map<std::shared_ptr<MaterialProperties::TexturePath>, ImageLoader>* pLoaderMap)
							{
								ImageLoader loader(pTexturePath->mTexturePath);

								std::lock_guard guard(mImageLocker);
								pLoaderMap->insert({ pTexturePath, std::move(loader) });
							}, pTexturePath, &loaderMap));

						break;
					}
				}
			}
		}

		const uint64 size = pOffScreenPass->GetBufferCount();
		pPackageSets.resize(size);
		pOcclusionPackageSets.reserve(size);

		// Enable or disable mip mapping.
		const bool bEnableMipMapping = true;

		for (auto& wireFrame : mAsset.GetWireFrames())
		{
			const Material material = wireFrame.GetMaterial();

			for (const auto pProperty : material.GetProperties())
			{
				if (pProperty->GetType() == MaterialProperties::MaterialType::TEXTURE_PATH)
				{
					const auto pTexturePath = std::static_pointer_cast<MaterialProperties::TexturePath>(pProperty);
					if (pTexturePath->mType != MaterialProperties::TextureType::Diffuse)
						continue;

					auto const& loader = loaderMap[pTexturePath];
					const auto pImage = loader.CreateImage(pApplication->GetDevice(), ImageType::TwoDimension, ImageUsage::Graphics, 1, !bEnableMipMapping);
					pImage->GenerateMipMaps();

					const auto pImageView = pImage->CreateImageView(0, pImage->GetLayerCount(), 0, pImage->GetMipLevels(), ImageUsage::Graphics);

					Flint::ImageSamplerSpecification samplerSpecification = {};
					samplerSpecification.mMaxLevelOfDetail = static_cast<float>(pImage->GetMipLevels());
					samplerSpecification.mCompareOperator = CompareOperator::Never;
					samplerSpecification.mAddressModeU = AddressMode::MirroredRepeat;
					samplerSpecification.mAddressModeV = AddressMode::MirroredRepeat;
					samplerSpecification.mAddressModeW = AddressMode::MirroredRepeat;
					samplerSpecification.mMaxAnisotrophy = 0.0f;
					samplerSpecification.bEnableAnisotropy = true;
					//samplerSpecification.mBorderColor = BorderColor::OpaqueBlackINT;
					const auto pSampler = pApplication->GetDevice()->CreateImageSampler(samplerSpecification);

					for (uint64 i = 0; i < size; i++)
					{
						auto pPackage = pPipeline->CreateResourcePackage(0);
						pPackage->BindResource(0, pUniformBuffer);
						pPackage->BindResource(1, pImage, pImageView, pSampler);
						pPackage->PrepareIfNecessary();

						pPackageSets[i].emplace_back(std::move(pPackage));
					}
				}
			}

			auto pPackage = pOcclusionPipeline->CreateResourcePackage(0);
			pPackage->BindResource(0, pUniformBuffer);
			pOcclusionPackageSets.emplace_back(std::move(pPackage));
		}
	}
}
