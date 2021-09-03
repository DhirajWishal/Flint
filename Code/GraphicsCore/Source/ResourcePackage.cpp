// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ResourcePackage.hpp"

namespace Flint
{
	ResourcePackage::ResourcePackage(const UI32 setIndex, const std::vector<UI32>& bufferBindings, const std::vector<UI32>& imageBindings)
		: mSetIndex(setIndex)
	{
		for (const UI32 binding : bufferBindings)
			mBufferBindings[binding] = {};

		for (const UI32 binding : imageBindings)
			mImageBindings[binding] = {};
	}

	void ResourcePackage::BindResource(const UI32 binding, const std::shared_ptr<Buffer>& pBuffer, const UI64 index, const UI64 offset)
	{
		if (mBufferBindings.find(binding) == mBufferBindings.end())
			throw std::invalid_argument("Submitted binding is not valid!");

		mBufferBindings[binding] = BufferBinding(pBuffer, index, offset);
		bIsUpdated = true;
	}

	void ResourcePackage::BindResource(const UI32 binding, const std::shared_ptr<ImageSampler>& pImageSampler, const std::shared_ptr<Image>& pImage, const UI64 index)
	{
		if (mImageBindings.find(binding) == mImageBindings.end())
			throw std::invalid_argument("Submitted binding is not valid!");

		mImageBindings[binding] = ImageBinding(pImageSampler, pImage, index);
		bIsUpdated = true;
	}

	const BufferBinding ResourcePackage::GetBufferBinding(const UI32 binding) const
	{
		if (mBufferBindings.find(binding) == mBufferBindings.end())
			throw std::invalid_argument("Submitted binding is not valid!");

		return mBufferBindings.at(binding);
	}

	const ImageBinding ResourcePackage::GetImageBinding(const UI32 binding) const
	{
		if (mImageBindings.find(binding) == mImageBindings.end())
			throw std::invalid_argument("Submitted binding is not valid!");

		return mImageBindings.at(binding);
	}
}