// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/DataType.hpp"
#include <algorithm>

#if defined(FLINT_PLATFORM_WINDOWS)
constexpr const char* CheckToken = "/";
constexpr const char* ReplaceToken = "\\";

#elif defined(FLINT_PLATFORM_LINUX)
constexpr const char* CheckToken = "\\";
constexpr const char* ReplaceToken = "/";

#endif

namespace Flint
{
	std::filesystem::path NormalizePath(std::filesystem::path path)
	{
		std::replace(path.begin(), path.end(), CheckToken, ReplaceToken);
		return path;
	}
}
