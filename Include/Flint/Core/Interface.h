// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#ifndef FLINT_BUILD_STATIC
#	ifdef FLINT_EXPORT
#		define FLINT_API	__declspec(dllexport)

#	else
#		define FLINT_API	__declspec(dllimport)

#	endif

#else
#	define FLINT_API

#endif