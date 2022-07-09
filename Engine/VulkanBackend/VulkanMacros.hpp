// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Errors/BackendError.hpp"

#define FLINT_VK_ASSERT(exp, msg)	if(exp != VK_SUCCESS) throw ::Flint::BackendError(msg)

#define GET_VK_BOOL(exp)			exp ? VK_TRUE : VK_FALSE