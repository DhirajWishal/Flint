// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/ProcessingPipeline/Nodes/ProcessingNode.hpp"
#include "Engine/ProcessingPipeline/ProcessingPipeline.hpp"

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

	UI32 ProcessingNode::GetBufferCount() const
	{
		return pProcessingPipeline->GetBufferCount();
	}

	std::shared_ptr<Device> ProcessingNode::GetDevice() const
	{
		return pProcessingPipeline->GetDevice();
	}

	std::shared_ptr<Display> ProcessingNode::GetDisplay() const
	{
		return pProcessingPipeline->GetDisplay();
	}
}
