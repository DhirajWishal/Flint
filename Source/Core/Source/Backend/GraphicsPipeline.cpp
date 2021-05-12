// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core\Backend\GraphicsPipeline.h"
#include "Core\Backend\Image.h"
#include "Core\Backend\Buffer.h"
#include "Core\Backend\RenderTarget.h"

namespace Flint
{
	namespace Backend
	{
		namespace GraphicsPipelinePresets
		{
			GraphicsPipelineSpecification CreateDefaultSpec()
			{
				GraphicsPipelineSpecification spec = {};
				//spec.bEnableSampleShading = false;
				//spec.bEnableAlphaCoverage = false;
				//spec.bEnableAlphaToOne = false;
				//spec.bEnableColorBlendLogic = false;
				//spec.bEnableDepthTest = true;
				//spec.bEnableDepthWrite = true;
				//spec.mRasterizerLineWidth = 1.0f;
				//spec.mMinSampleShading = 0.0f;
				//spec.mColorBlendConstants[0] = 0.0f;
				//spec.mColorBlendConstants[1] = 0.0f;
				//spec.mColorBlendConstants[2] = 0.0f;
				//spec.mColorBlendConstants[3] = 0.0f;

				return spec;
			}
		}
	}
}