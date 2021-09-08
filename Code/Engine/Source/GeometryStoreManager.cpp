// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/GeometryStoreManager.hpp"
#include "Engine/AssetLoader.hpp"

namespace Flint
{
	void GeometryStoreManager::LoadStaticGeometry(ComponentHandle<Components::StaticModel>& staticModel, const std::filesystem::path& asset, const std::shared_ptr<Shader> pShader, const VertexDescriptor& vertexDescriptor)
	{
		const auto inputAttributes = pShader->GetInputAttributes();
		if (mGeometryStoreMap.find(inputAttributes) == mGeometryStoreMap.end())
			CreateNewGeometryStore(inputAttributes);

		auto pComponent = staticModel.GetComponent();
		pComponent->pGeometryStore = mGeometryStoreMap[inputAttributes];

		AssetLoader assetLoader(pComponent->pGeometryStore, asset, vertexDescriptor);
		pComponent->mWireFrames = assetLoader.GetWireFrames();
	}

	void GeometryStoreManager::CreateNewGeometryStore(const TShaderAttributeMap& inputAttributes)
	{
		mGeometryStoreMap[inputAttributes] = pDefaultDevice->CreateGeometryStore(inputAttributes, sizeof(UI32));
	}
}