// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ResourceBinder.hpp"

namespace Flint
{
	ResourceBinder::ResourceBinder(const std::shared_ptr<GraphicsPipeline>& pPipeline)
	{
		if (!pPipeline)
			throw std::invalid_argument("Pipeline pointer should not be null!");
	}

	ResourceBinder::ResourceBinder(const std::shared_ptr<ComputePipeline>& pPipeline)
	{
		if (!pPipeline)
			throw std::invalid_argument("Pipeline pointer should not be null!");
	}

	void ResourceBinder::BindResource(const UI32 binding, const std::shared_ptr<Buffer>& pBuffer, const UI64 offset)
	{
		mBufferMap[binding] = BufferBinding(pBuffer, offset);
		bShouldUpdate = true;
	}

	void ResourceBinder::BindResource(const UI32 binding, const std::shared_ptr<Image>& pImage, const std::shared_ptr<ImageSampler>& pImageSampler)
	{
		mImageMap[binding] = ImageBinding(pImage, pImageSampler);
		bShouldUpdate = true;
	}
}