// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"

#define FLINT_LOGGER_COLOR_BLUE			{ 0x1b, '[', '1', ';', '3', '4', 'm', 0 }
#define FLINT_LOGGER_COLOR_GREEN		{ 0x1b, '[', '1', ';', '9', '2', 'm', 0 }
#define FLINT_LOGGER_COLOR_YELLOW		{ 0x1b, '[', '1', ';', '9', '3', 'm', 0 }
#define FLINT_LOGGER_COLOR_RED			{ 0x1b, '[', '1', ';', '3', '1', 'm', 0 }
#define FLINT_LOGGER_COLOR_DARK_RED		{ 0x1b, '[', '4', ';', '3', '1', 'm', 0 }
#define FLINT_LOGGER_COLOR_DEFAULT		{ 0x1b, '[', '0', ';', '3', '9', 'm', 0 }

namespace Flint
{
	/**
	 * The logger is a namespace which contains functions which allows the user to log messages to the console.
	 * 
	 * Formatting command is set using the # tag. This tag can include,
	 * * 0: 8 bit integers.
	 * * 1: 16 bit integers.
	 * * 2: 32 bit integers.
	 * * 3: 64 bit integers.
	 * * 4: float.
	 * * 5: double.
	 * * 6: string (wchar*).
	 * * 7: string (char*).
	 * * 8: pointer.
	 * 
	 * Example: LogError("My age is #0.", 19);
	 */
	namespace Logger
	{
		/**
		 * Switch from one color to another.
		 * Once switched, make sure to color it back to default or else the console will be colored to that specific color.
		 *
		 * @param color: The 8 character color code.
		 */
		void SwitchColor(const wchar(&color)[8]);

		/**
		 * Print the current time to the console.
		 */
		void PrintTime();

		/**
		 * Print the log string.
		 *
		 * @param format: The string to print with or without formatting.
		 * @param VA_ARGS: The arguments for formatting.
		 */
		void Print(const wchar* format, ...);

		/**
		 * Log debug information.
		 * Color: Blue.
		 *
		 * @param format: The string to be printed with or without formatting.
		 * @param args: The arguments to be formatted.
		 */
		template<class... Args>
		void LogDebug(const wchar* format, Args&&... args)
		{
			SwitchColor(FLINT_LOGGER_COLOR_BLUE);
			PrintTime();
			Print(format, args...);
			SwitchColor(FLINT_LOGGER_COLOR_DEFAULT);
		}

		/**
		 * Log information.
		 * Color: Green.
		 *
		 * @param format: The string to be printed with or without formatting.
		 * @param args: The arguments to be formatted.
		 */
		template<class... Args>
		void LogInfo(const wchar* format, Args&&... args)
		{
			SwitchColor(FLINT_LOGGER_COLOR_GREEN);
			PrintTime();
			Print(format, args...);
			SwitchColor(FLINT_LOGGER_COLOR_DEFAULT);
		}

		/**
		 * Log warning information.
		 * Color: Yellow.
		 *
		 * @param format: The string to be printed with or without formatting.
		 * @param args: The arguments to be formatted.
		 */
		template<class... Args>
		void LogWarning(const wchar* format, Args&&... args)
		{
			SwitchColor(FLINT_LOGGER_COLOR_YELLOW);
			PrintTime();
			Print(format, args...);
			SwitchColor(FLINT_LOGGER_COLOR_DEFAULT);
		}

		/**
		 * Log error information.
		 * Color: Red.
		 *
		 * @param format: The string to be printed with or without formatting.
		 * @param args: The arguments to be formatted.
		 */
		template<class... Args>
		void LogError(const wchar* format, Args&&... args)
		{
			SwitchColor(FLINT_LOGGER_COLOR_RED);
			PrintTime();
			Print(format, args...);
			SwitchColor(FLINT_LOGGER_COLOR_DEFAULT);
		}

		/**
		 * Log fatal information.
		 * Color: Dark Red.
		 *
		 * @param format: The string to be printed with or without formatting.
		 * @param args: The arguments to be formatted.
		 */
		template<class... Args>
		void LogFatal(const wchar* format, Args&&... args)
		{
			SwitchColor(FLINT_LOGGER_COLOR_DARK_RED);
			PrintTime();
			Print(format, args...);
			SwitchColor(FLINT_LOGGER_COLOR_DEFAULT);
		}
	}
}

#ifdef FLINT_DEBUG

#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)

#define FLINT_LOG_INFO(format, ...)			::Flint::Logger::LogInfo(format, __VA_ARGS__);
#define FLINT_LOG_DEBUG(format, ...)		::Flint::Logger::LogDebug(format, __VA_ARGS__);
#define FLINT_LOG_WARN(format, ...)			::Flint::Logger::LogWarning(format, __VA_ARGS__);
#define FLINT_LOG_ERROR(format, ...)		::Flint::Logger::LogError(format, __VA_ARGS__);
#define FLINT_LOG_FATAL(format, ...)		::Flint::Logger::LogFatal(format, __VA_ARGS__, WIDE1(__FILE__), __LINE__), __debugbreak();

#define FLINT_ASSERT(exp, ...)				if(!exp) FLINT_LOG_FATAL(__VA_ARGS__)

#else

#define FLINT_LOG_INFO(...)
#define FLINT_LOG_DEBUG(...)
#define FLINT_LOG_WARN(...)
#define FLINT_LOG_ERROR(...)
#define FLINT_LOG_FATAL(...)

#define FLINT_ASSERT(exp, ...)		

#endif // FLINT_DEBUG

