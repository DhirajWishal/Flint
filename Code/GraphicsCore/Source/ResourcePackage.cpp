// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ResourcePackage.hpp"

namespace Flint
{
	ResourcePackage::ResourcePackage(const std::shared_ptr<ResourcePackager>& pPackager, const std::vector<uint32>& bufferBindings, const std::vector<uint32>& imageBindings)
		: pPackager(pPackager)
	{
		for (const uint32 binding : bufferBindings)
			mBufferBindings[binding] = {};

		for (const uint32 binding : imageBindings)
			mImageBindings[binding] = {};
	}

	void ResourcePackage::BindResource(const uint32 binding, const std::shared_ptr<Buffer>& pBuffer, const uint64 offset)
	{
		if (mBufferBindings.find(binding) == mBufferBindings.end())
			throw std::invalid_argument("Submitted binding is not valid!");

		mBufferBindings[binding].push_back(BufferBinding(pBuffer, offset));
		bIsUpdated = true;
	}

	void ResourcePackage::BindResource(const uint32 binding, const std::shared_ptr<Image>& pImage, const std::shared_ptr<ImageView>& pImageView, const std::shared_ptr<ImageSampler>& pImageSampler, const ImageUsage usage)
	{
		if (mImageBindings.find(binding) == mImageBindings.end())
			throw std::invalid_argument("Submitted binding is not valid!");

		mImageBindings[binding].push_back(ImageBinding(pImage, pImageView, pImageSampler, usage));
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

	const std::vector<BufferBinding> ResourcePackage::GetBufferBindings(const uint32 binding) const
	{
		if (mBufferBindings.find(binding) == mBufferBindings.end())
			throw std::invalid_argument("Submitted binding is not valid!");

		return mBufferBindings.at(binding);
	}

	const std::vector<ImageBinding> ResourcePackage::GetImageBindings(const uint32 binding) const
	{
		if (mImageBindings.find(binding) == mImageBindings.end())
			throw std::invalid_argument("Submitted binding is not valid!");

		return mImageBindings.at(binding);
	}
}