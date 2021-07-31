// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "AssetLoader.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../ThirdParty/tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../ThirdParty/stb_image.h"

#include <unordered_map>

namespace std
{
	template<>
	struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			return ((hash<glm::vec3>::_Do_hash(vertex.pos) ^ (hash<glm::vec3>::_Do_hash(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>::_Do_hash(vertex.texCoord) << 1);
		}
	};
}

std::pair<std::vector<Vertex>, std::vector<UI32>> LoadAsset(const std::filesystem::path& path)
{
	std::vector<Vertex> vertexes;
	std::vector<UI32> indexes;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.string().c_str()))
		throw std::runtime_error(warn + err);

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			if (attrib.texcoords.size())
			{
				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertexes.size());

				vertexes.push_back(vertex);
			}

			indexes.push_back(uniqueVertices[vertex]);
		}
	}

	return std::pair<std::vector<Vertex>, std::vector<UI32>>{ vertexes, indexes };
}

ImageData LoadImage(const std::filesystem::path& path)
{
	I32 texWidth = 0, texHeight = 0, texChannels = 0;
	stbi_uc* pixels = stbi_load(path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	ImageData data;
	data.pImageData = pixels;
	data.mExtent = Flint::FBox3D(texWidth, texHeight, 1);
	return data;
}

void DestroyImage(ImageData data)
{
	stbi_image_free(data.pImageData);
}
