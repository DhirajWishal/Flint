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

	boost::shared_ptr<Flint::Buffer> pVertexBuffer = nullptr;
	UI64 mVertexCount = 0;

	boost::shared_ptr<Flint::Buffer> pIndexBuffer = nullptr;
	UI64 mIndexCount = 0;

	boost::container::vector<DrawInstance> mDrawInstances;
};

enum class VertexAttributeType : UI8 {
	UNDEFINED,
	POSITION,
	NORMAL,
	COLOR_0,
	COLOR_1,
	COLOR_2,
	COLOR_3,
	TEXTURE_COORDINATES_0,
	TEXTURE_COORDINATES_1,
	TEXTURE_COORDINATES_2,
	TEXTURE_COORDINATES_3,
	TEXTURE_COORDINATES_4,
	TEXTURE_COORDINATES_5,
	TEXTURE_COORDINATES_6,
	TEXTURE_COORDINATES_7,
	UV_COORDINATES,
	TANGENT,
	BITANGENT,
	INTEGRITY,
	BONE_ID,
	BONE_WEIGHT,
	CUSTOM
};

struct VertexAttribute
{
	VertexAttribute() = default;
	VertexAttribute(UI8 size, VertexAttributeType type) : mAttributeSize(size), mType(type) {}

	UI8 mAttributeSize = 0;
	VertexAttributeType mType = VertexAttributeType::UNDEFINED;
};

boost::container::vector<VertexAttribute> GetDefaultVertexAttributes();

Asset ImportAsset(const boost::shared_ptr<Flint::Device>& pDevice, const boost::filesystem::path& path, const boost::container::vector<VertexAttribute>& attributes = GetDefaultVertexAttributes());

struct ImageData
{
	void* pImageData = nullptr;
	Flint::FBox3D mExtent;
};

ImageData LoadImage(const boost::filesystem::path& path);
void DestroyImage(ImageData data);