// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/Components/StaticModel.hpp"
#include "Engine/ComponentSystem/ComponentRegistry.hpp"
#include "GraphicsBase.hpp"
#include "Graphics/AssetLoader.hpp"

namespace Flint
{
	/**
	 * Geometry store manager object.
	 * This object manages all the geometry stores used by the client.
	 */
	class GeometryStoreManager : public GraphicsBase
	{
	public:
		GeometryStoreManager() = default;

		/**
		 * Load static geometry to a static model.
		 * 
		 * @param staticModel The static model to load the static data to.
		 * @param asset The asset file to load.
		 * @param pShader The shader to which the asset is drawn to.
		 * @param vertexDescriptor The vertex descriptor specifying what to load.
		 */
		void LoadStaticGeometry(ComponentHandle<Components::StaticModel>& staticModel, const std::filesystem::path& asset, const std::shared_ptr<Shader> pShader, const VertexDescriptor& vertexDescriptor);

	private:
		/**
		 * Create a new geometry store.
		 * 
		 * @param inputAttributes The input attributes of the geometry store.
		 */
		void CreateNewGeometryStore(const std::vector<ShaderAttribute>& inputAttributes);

	protected:
		std::unordered_map<std::vector<ShaderAttribute>, std::shared_ptr<GeometryStore>> mGeometryStoreMap = {};
	};
}