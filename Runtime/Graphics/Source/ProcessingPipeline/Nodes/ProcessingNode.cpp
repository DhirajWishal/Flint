// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/ProcessingPipeline/Nodes/ProcessingNode.hpp"
#include "Graphics/ProcessingPipeline/ProcessingPipeline.hpp"

namespace Flint
{
	ProcessingNode::ProcessingNode(ProcessingPipeline* pProcessingPipeline)
		: pProcessingPipeline(pProcessingPipeline)
	{
		if (!pProcessingPipeline)
			throw std::invalid_argument("Processing Pipeline pointer should not be null!");
	}

	MultiSampleCount ProcessingNode::GetMultiSampleCount() const
	{
		return pProcessingPipeline->GetMultiSampleCount();
	}

	FBox2D ProcessingNode::GetExtent() const
	{
		return pProcessingPipeline->GetExtent();
	}

	uint32_t ProcessingNode::GetBufferCount() const
	{
		return pProcessingPipeline->GetBufferCount();
	}

	Device* ProcessingNode::GetDevice() const
	{
		return pProcessingPipeline->GetDevice();
	}

	std::shared_ptr<Display> ProcessingNode::GetDisplay() const
	{
		return pProcessingPipeline->GetDisplay();
	}
}
