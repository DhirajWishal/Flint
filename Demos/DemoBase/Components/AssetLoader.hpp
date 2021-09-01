// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Shader.hpp"
#include "Flint/Buffer.hpp"
#include <glm/glm.hpp>

struct Asset
{
	struct DrawInstance
	{
		DrawInstance() = default;
		DrawInstance(UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount)
			: mVertexOffset(vertexOffset), mVertexCount(vertexCount), mIndexOffset(indexOffset), mIndexCount(indexCount) {}

		glm::mat4 mTransform = glm::mat4(1.0f);

		std::string mName = "";

		UI64 mVertexOffset = 0;
		UI64 mVertexCount = 0;

		UI64 mIndexOffset = 0;
		UI64 mIndexCount = 0;
	};

	std::shared_ptr<Flint::Buffer> pVertexBuffer = nullptr;
	UI64 mVertexCount = 0;

	std::shared_ptr<Flint::Buffer> pIndexBuffer = nullptr;
	UI64 mIndexCount = 0;

	std::vector<DrawInstance> mDrawInstances;
};

enum class VertexAttributeType : UI8 {
	Undefined,
	Position,
	Normal,
	ColorZero,
	ColorOne,
	ColorTwo,
	ColorThree,
	TextureCoordinatesZero,
	TextureCoordinatesOne,
	TextureCoordinatesTwo,
	TextureCoordinatesThree,
	TextureCoordinatesFour,
	TextureCoordinatesFive,
	TextureCoordinatesSix,
	TextureCoordinatesSeven,
	UVCoordinates,
	Tangent,
	Bitangent,
	INTEGRITY,
	BoneID,
	BoneWeight,
	CUSTOM
};

struct VertexAttribute
{
	VertexAttribute() = default;
	VertexAttribute(UI8 size, VertexAttributeType type) : mAttributeSize(size), mType(type) {}

	UI8 mAttributeSize = 0;
	VertexAttributeType mType = VertexAttributeType::Undefined;
};

std::vector<VertexAttribute> GetDefaultVertexAttributes();

Asset ImportAsset(const std::shared_ptr<Flint::Device>& pDevice, const std::filesystem::path& path, const std::vector<VertexAttribute>& attributes = GetDefaultVertexAttributes());

struct ImageData
{
	void* pImageData = nullptr;
	Flint::FBox3D mExtent;
};

ImageData LoadImage(const std::filesystem::path& path);
void DestroyImage(ImageData data);