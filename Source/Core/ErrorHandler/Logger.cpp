// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/ErrorHandler/Logger.h"

#include <cstdio>
#include <cstdarg>
#include <time.h>

namespace Flint
{
	namespace Logger
	{
		void SwitchColor(const wchar(&color)[8]) { wprintf(TEXT("%s"), color); }

		void PrintTime()
		{
			_tzset();
			wchar pBuffer[12];
			_wstrtime_s(pBuffer);
			wprintf(TEXT("{ %s } "), pBuffer);
		}

		void Print(const wchar* format, ...)
		{
			va_list args = {};
			va_start(args, format);

			while (*format != '\0')
			{
				if (*format == '#')
				{
					++format;
					switch (*format)
					{
					case '0': wprintf(TEXT("%d"), va_arg(args, int32_t)); break;
					case '1': wprintf(TEXT("%d"), va_arg(args, int32_t)); break;
					case '2': wprintf(TEXT("%d"), va_arg(args, int32_t)); break;
					case '3': wprintf(TEXT("%lld"), va_arg(args, int64_t)); break;
					case '4': wprintf(TEXT("%f"), va_arg(args, float)); break;
					case '5': wprintf(TEXT("%lf"), va_arg(args, double)); break;
					case '6': wprintf(TEXT("%s"), va_arg(args, const wchar*)); break;
					case '7': printf("%s", va_arg(args, const char*)); break;
					case '8': wprintf(TEXT("%p"), va_arg(args, void*)); break;
					default: wprintf(TEXT("%c"), *format); break;
					}
				}
				else
					wprintf(TEXT("%c"), *format);

				++format;
			}

			wprintf(TEXT("\n"));
		}
	}
}