// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/DrawEntry.hpp"
#include "Core/Errors/InvalidArgumentError.hpp"

namespace Flint
{
	DrawEntry::DrawEntry(const std::shared_ptr<Entity>& pEntity)
		: m_pEntity(pEntity)
	{
		if (!pEntity)
			throw InvalidArgumentError("The entity pointer should not be null!");
	}
}