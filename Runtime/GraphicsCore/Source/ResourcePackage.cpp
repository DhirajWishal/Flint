// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ResourcePackage.hpp"

namespace Flint
{
	ResourcePackage::ResourcePackage(const std::shared_ptr<ResourcePackager>& pPackager, const std::vector<uint32_t>& bufferBindings, const std::vector<uint32_t>& imageBindings)
		: pPackager(pPackager)
	{
		for (const uint32_t binding : bufferBindings)
			mBufferBindings[binding] = {};

		for (const uint32_t binding : imageBindings)
			mImageBindings[binding] = {};
	}

	void ResourcePackage::BindResource(const uint32_t binding, const Buffer* pBuffer, const uint64_t offset)
	{
		if (mBufferBindings.find(binding) == mBufferBindings.end())
			throw std::invalid_argument("Submitted binding is not valid!");

		mBufferBindings[binding].emplace_back(BufferBinding(pBuffer, offset));
		bIsUpdated = true;
	}

	void ResourcePackage::BindResource(const uint32_t binding, const Image* pImage, const ImageView* pImageView, const ImageSampler* pImageSampler, const ImageUsage usage)
	{
		if (mImageBindings.find(binding) == mImageBindings.end())
			throw std::invalid_argument("Submitted binding is not valid!");

		mImageBindings[binding].emplace_back(ImageBinding(pImage, pImageView, pImageSampler, usage));
		bIsUpdated = true;
	}

	void ResourcePackage::ClearBufferResources()
	{
		for (auto& [binding, resources] : mBufferBindings)
			resources.clear();
	}

	void ResourcePackage::ClearImageResources()
	{
		for (auto& [binding, resources] : mImageBindings)
			resources.clear();
	}

	const std::vector<BufferBinding> ResourcePackage::GetBufferBindings(const uint32_t binding) const
	{
		if (mBufferBindings.find(binding) == mBufferBindings.end())
			throw std::invalid_argument("Submitted binding is not valid!");

		return mBufferBindings.at(binding);
	}

	const std::vector<ImageBinding> ResourcePackage::GetImageBindings(const uint32_t binding) const
	{
		if (mImageBindings.find(binding) == mImageBindings.end())
			throw std::invalid_argument("Submitted binding is not valid!");

		return mImageBindings.at(binding);
	}
}