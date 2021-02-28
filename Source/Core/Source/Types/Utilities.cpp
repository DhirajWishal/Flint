// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Types/Utilities.h"

#include <codecvt>
#include <cstring>

namespace Flint
{
	std::string WStringToString(const std::wstring& string)
	{
		std::wstring_convert<std::codecvt_utf8<wchar>, wchar> convert;
		return convert.to_bytes(string);

		std::string str;
		str.resize(string.length());

		std::mbstate_t state = std::mbstate_t();
		const wchar* pString = string.c_str();

		std::wcsrtombs(str.data(), &pString, string.length(), &state);
		return str;
	}

	std::wstring StringToWString(const std::string& string)
	{
		std::wstring str;
		str.resize(string.length());

		std::mbstate_t state = std::mbstate_t();
		const char* pString = string.c_str();

		std::mbsrtowcs(str.data(), &pString, string.length(), &state);
		return str;
	}
}
