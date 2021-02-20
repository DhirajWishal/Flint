// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Backend/CommandBufferManager.h"

namespace Flint
{
	namespace Backend
	{
		void Flint::Backend::CommandBufferManager::ResolveCommandBuffers(UI32 count)
		{
			for (UI32 i = 0; i < count; i++)
			{
				CommandBuffer buffer = {};
				buffer.pParent = this;
				buffer.mIndex = i;
				buffer.mState = CommandBuffer::State::VALID;

				INSERT_INTO_VECTOR(mCommandBuffers, buffer);
			}
		}
	}
}