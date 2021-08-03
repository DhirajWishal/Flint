// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanUtilities.hpp"

#ifdef FLINT_DEBUG
#	define FLINT_VK_ASSERT(expression)	::Flint::VulkanBackend::Utilities::CheckResult(expression)

#else
#	define FLINT_VK_ASSERT(expression)	expression

#endif // FLINT_DEBUG

#define GET_VK_BOOL(value)		(value ? VK_TRUE : VK_FALSE)
