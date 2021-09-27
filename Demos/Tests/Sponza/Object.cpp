// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Object.hpp"

#include "TestBase/GraphicsScene.hpp"
#include "Engine/ShaderCompiler.hpp"
#include "Engine/ImageLoader.hpp"
#include "Engine/Tools/CubeMapGenerator.hpp"

#include <glm/gtx/transform.hpp>
#include <optick.h>

namespace Flint
{
	void Object::Initialize(Application* pApplication)
	{
		OPTICK_EVENT();

		this->pApplication = pApplication;

		pDynamicStates = std::make_shared<DynamicStateContainer>();
		pUniformBuffer = pApplication->GetDevice()->CreateBuffer(BufferType::Uniform, sizeof(ModelViewProjection));

		mMatrix.mModelMatrix = glm::scale(mMatrix.mModelMatrix, glm::vec3(0.005f));

		CreatePipeline();
		LoadAsset();
		LoadTextures();
	}

	void Object::Update(UI64 delta, Camera* pCamera)
	{
		OPTICK_EVENT();

		auto mat = pCamera->GetMatrix();
		mMatrix.mViewMatrix = mat.mViewMatrix;
		mMatrix.mProjectionMatrix = mat.mProjectionMatrix;

		ImGui::Begin("Settings");
		ImGui::SliderFloat("Bias", &mMatrix.mMipBias, 0, 10);
		ImGui::End();

		CopyToBuffer(pUniformBuffer, mMatrix);
	}

	void Object::Draw(const std::shared_ptr<CommandBuffer>& pCommandBuffer, const UI32 index)
	{
		OPTICK_EVENT();

		pCommandBuffer->BindGraphicsPipeline(pPipeline.get());
		pCommandBuffer->BindGeometryStore(mAsset.GetGeometryStore().get());
		pCommandBuffer->BindDynamicStates(pPipeline.get(), pDynamicStates.get());

		const UI64 count = mAsset.GetWireFrames().size();
		auto& wireFrames = mAsset.GetWireFrames();
		for (UI64 i = 0; i < count; i++)
		{
			auto& wireFrame = wireFrames[i];
			auto& pPackage = pPackageSets[index][i];

			pCommandBuffer->BindResourcePackage(pPipeline.get(), pPackage.get());
			pCommandBuffer->IssueDrawCall(wireFrame);
		}
	}

	void Object::Terminate()
	{
		pUniformBuffer->Terminate();
		pPipeline->Terminate();
		pPackageSets.clear();

		mAsset.Clear();
	}

	void Object::CreatePipeline()
	{
		OPTICK_EVENT();

		auto pDevice = pApplication->GetDevice();
		std::shared_ptr<Shader> pVertexShader = nullptr;
		std::shared_ptr<Shader> pFragmentShader = nullptr;

		if (!std::filesystem::exists("Flint\\Shaders\\Object.vert.fsc"))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path("E:\\Flint\\Demos\\Tests\\Sponza\\Shaders\\Object\\Object.vert"), ShaderCodeType::GLSL, ShaderType::Vertex);
			pVertexShader = shaderCompiler.CreateShader(pDevice);
			pVertexShader->CreateCache("Flint\\Shaders\\Object.vert.fsc");
		}
		else
			pVertexShader = pDevice->CreateShader(ShaderType::Vertex, std::filesystem::path("Flint\\Shaders\\Object.vert.fsc"));

		if (!std::filesystem::exists("Flint\\Shaders\\Object.frag.fsc"))
		{
			ShaderCompiler shaderCompiler(std::filesystem::path("E:\\Flint\\Demos\\Tests\\Sponza\\Shaders\\Object\\Object.frag"), ShaderCodeType::GLSL, ShaderType::Fragment);
			pFragmentShader = shaderCompiler.CreateShader(pDevice);
			pFragmentShader->CreateCache("Flint\\Shaders\\Object.frag.fsc");
		}
		else
			pFragmentShader = pDevice->CreateShader(ShaderType::Fragment, std::filesystem::path("Flint\\Shaders\\Object.frag.fsc"));

		Flint::GraphicsPipelineSpecification specification = {};
		specification.mRasterizationSamples = MultiSampleCount::One;
		//specification.mRasterizationSamples = pApplication->pDevice->GetSupportedMultiSampleCount();
		specification.mDynamicStateFlags = Flint::DynamicStateFlags::ViewPort | Flint::DynamicStateFlags::Scissor;

		specification.mVertexInputAttributeMap[0] = pVertexShader->GetInputAttributes();
		pPipeline = pApplication->GetGraphicsScene("Default")->CreateGraphicsPipeline("Object", pVertexShader, pFragmentShader, specification);

		pApplication->CreateGeometryStore("Object", pVertexShader->GetInputAttributes(), sizeof(UI32));

		auto windowExtent = pApplication->GetGraphicsScene("Default")->GetDisplay()->GetExtent();
		pDynamicStates->SetViewPort(Flint::FExtent2D<float>{static_cast<float>(windowExtent.mWidth), static_cast<float>(windowExtent.mHeight)}, Flint::FExtent2D<float>(0.0f, 1.0f), { 0.0f, 0.0f });
		pDynamicStates->SetScissor(windowExtent, { 0, 0 });
	}

	void Object::LoadAsset()
	{
		OPTICK_EVENT();

		Flint::VertexDescriptor vDescriptor = {};
		vDescriptor.mAttributes.push_back(Flint::VertexAttribute(sizeof(float) * 3, Flint::InputAttributeType::Position));
		//vDescriptor.mAttributes.push_back(Flint::VertexAttribute(sizeof(float) * 3, Flint::InputAttributeType::ColorZero));
		vDescriptor.mAttributes.push_back(Flint::VertexAttribute(sizeof(float) * 2, Flint::InputAttributeType::TextureCoordinatesZero));

		mAsset = Asset("E:\\Projects\\Lighter\\Assets\\2.0\\Sponza\\glTF\\Sponza.gltf", pApplication->GetGeometryStore("Object"), vDescriptor);
	}

	void Object::LoadTextures()
	{
		OPTICK_EVENT();

		const UI64 size = pApplication->GetGraphicsScene("Default")->pRenderTarget->GetBufferCount();
		pPackageSets.resize(size);

		for (auto& wireFrame : mAsset.GetWireFrames())
		{
			const Material material = wireFrame.GetMaterial();

			for (const auto pProperty : material.GetProperties())
			{
				if (pProperty->GetType() == MaterialProperties::MaterialType::TEXTURE_DATA)
				{
					auto pTexturePath = std::static_pointer_cast<MaterialProperties::TextureData>(pProperty);
					if (pTexturePath->mType != MaterialProperties::TextureType::Diffuse)
						continue;

					auto pImage = pTexturePath->CreateImage(pApplication->GetDevice(), ImageType::TwoDimension, ImageUsage::Graphics, 1, 1);

					Flint::ImageSamplerSpecification samplerSpecification = {};
					samplerSpecification.mMaxLevelOfDetail = static_cast<float>(pImage->GetMipLevels());
					samplerSpecification.mBorderColor = BorderColor::TransparentBlackFLOAT;
					auto pSampler = pApplication->GetDevice()->CreateImageSampler(samplerSpecification);

					for (UI64 i = 0; i < size; i++)
					{
						auto pPackage = pPipeline->CreateResourcePackage(0);
						pPackage->BindResource(0, pUniformBuffer);
						pPackage->BindResource(1, pSampler, pImage);
						pPackage->PrepareIfNecessary();

						pPackageSets[i].push_back(std::move(pPackage));
					}

					break;
				}
			}
		}
	}
}