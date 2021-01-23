// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"

namespace Flint
{
	/**
	 *  The logger is a namespace which contains functions which allows the user to log messages to the console.
	 */
	namespace Logger
	{
		/**
		 * Log basic information to the console.
		 * Color: Green.
		 *
		* @param message: The message to be logged.
		 */
		void LogInfo(const wchar* message);

		/**
		 * Log a warning message to the console.
		 * Color: Yellow.
		 *
		* @param message: The message to be logged.
		 */
		void LogWarn(const wchar* message);

		/**
		 * Log an error message to the console.
		 * Color: Red.
		 *
		* @param message: The message to be logged.
		 */
		void LogError(const wchar* message);

		/**
		 * Log a fatal error message to the console.
		 * Color: Red + underlined.
		 *
		* @param message: The message to be logged.
		* @param file: The file the error had occurred.
		* @param line: The line of the error.
		 */
		void LogFatal(const wchar* message, const wchar* file, UI32 line);

		/**
		 * Log a debug information to the console.
		 * Color: Blue.
		 *
		* @param message: The message to be logged.
		 */
		void LogDebug(const wchar* message);
	}
}

#ifdef FLINT_DEBUG

#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)

#define FLINT_LOG_INFO(...)			::Flint::Logger::LogInfo(__VA_ARGS__);
#define FLINT_LOG_DEBUG(...)		::Flint::Logger::LogDebug(__VA_ARGS__);
#define FLINT_LOG_WARN(...)			::Flint::Logger::LogWarn(__VA_ARGS__);
#define FLINT_LOG_ERROR(...)		::Flint::Logger::LogError(__VA_ARGS__);
#define FLINT_LOG_FATAL(...)		::Flint::Logger::LogFatal(__VA_ARGS__, WIDE1(__FILE__), __LINE__), __debugbreak();

#define FLINT_ASSERT(exp, ...)		if(exp)	FLINT_LOG_FATAL(__VA_ARGS__)

#else

#define FLINT_LOG_INFO(...)
#define FLINT_LOG_DEBUG(...)
#define FLINT_LOG_WARN(...)
#define FLINT_LOG_ERROR(...)
#define FLINT_LOG_FATAL(...)

#define FLINT_ASSERT(exp, ...)	//

#endif // FLINT_DEBUG

