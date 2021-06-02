// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DataType.h"

#include <string>
#include <stdexcept>

#define FLINT_THROW_LOGIC_ERROR										throw std::logic_error
#define FLINT_THROW_DOMAIN_ERROR									throw std::domain_error
#define FLINT_THROW_INVALID_ARGUMENT								throw std::invalid_argument
#define FLINT_THROW_LENGTH_ERROR									throw std::length_error
#define FLINT_THROW_OUT_OF_RANGE									throw std::out_of_range
#define FLINT_THROW_RUNTIME_ERROR									throw std::runtime_error
#define FLINT_THROW_RANGE_ERROR										throw std::range_error
#define FLINT_THROW_OVERFLOW_ERROR									throw std::overflow_error
#define FLINT_THROW_UNDERFLOW_ERROR									throw std::underflow_error

#ifdef FLINT_DEBUG
#define FLINT_ASSERT(expression, condition, error)					if (expression != condition) error  
#define FLINT_ASSERT_DISCARDABLE(expression, condition, error)		if (expression != condition) error;

#else
#define FLINT_ASSERT(expression, condition, error)					expression
#define FLINT_ASSERT_DISCARDABLE(expression, condition, error)	

#endif // FLINT_DEBUG
