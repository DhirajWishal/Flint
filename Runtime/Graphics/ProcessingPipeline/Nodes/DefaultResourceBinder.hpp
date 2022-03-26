// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ProcessingNode.hpp"

namespace Flint
{
	/**
	 * Flint default resource binder.
	 * The default resource binder is used to bind resources to the command pipeline.
	 */
	class DefaultResourceBinder : public ProcessingNode
	{
	public:
		DefaultResourceBinder(ProcessingPipeline* pPipeline);
	};
}