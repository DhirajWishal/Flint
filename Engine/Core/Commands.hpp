// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Types.hpp"

#include <future>
#include <deque>
#include <variant>

namespace Flint
{
	FLINT_DEFINE_HANDLE(InstanceHandle);

	namespace Commands
	{
		/**
		 * Command structure.
		 * This contains the main promise which will be passed to the user as a future.
		 *
		 * @tparam Type The type which will be returned by the promise.
		 */
		template<class Type>
		struct Command { std::promise<Type> m_Promise; };

		/**
		 * Create instance command.
		 */
		struct CreateInstance final : public Command<InstanceHandle>
		{
			std::string m_ApplicationName;

			uint32_t m_ApplicationVersion;
			bool m_EnableValidation = true;
		};

		/**
		 * Terminate command.
		 */
		struct Terminate final : public Command<void> {};
	}

	/**
	 * Command variant type.
	 * We send commands using variants because it's just easier to do it that way especially that all the commands are in the form of PODs.
	 */
	using CommandVariant = std::variant<
		Commands::Terminate
	>;

	/**
	 * Command storage.
	 * This is the actual command storage that will be used.
	 */
	using CommandStorage = std::deque<CommandVariant>;
}