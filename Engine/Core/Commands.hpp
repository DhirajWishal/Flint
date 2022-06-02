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
	FLINT_DEFINE_HANDLE(DeviceHandle);

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

		/**
		 * Create device command.
		 */
		struct CreateDevice final : public Command<DeviceHandle> {};

		/**
		 * Destroy device command.
		 */
		struct DestroyDevice final : public Command<void>
		{
			DeviceHandle m_DeviceHandle;
		};
	}

	/**
	 * Command variant type.
	 * We send commands using variants because it's just easier to do it that way especially that all the commands are in the form of PODs.
	 */
	using CommandVariant = std::variant<
		Commands::Terminate,
		Commands::CreateDevice,
		Commands::DestroyDevice
	>;

	/**
	 * Command variant index struct.
	 */
	template<class Command, class Variant>
	struct CommandVariantIndex;

	/**
	 * Command variant index struct specialization.
	 */
	template<class Command, class... Types>
	struct CommandVariantIndex<Command, std::variant<Command, Types...>>
	{
		static const uint32_t Index = 0;
	};

	/**
	 * Command variant index struct specialization.
	 */
	template<class Command1, class Command2, class... Types>
	struct CommandVariantIndex<Command1, std::variant<Command2, Types...>>
	{
		static const uint32_t Index = 1 + CommandVariantIndex<Command1, std::variant<Types...>>::Index;
	};

	/**
	 * Command storage class.
	 * This is the actual command storage that will be used.
	 */
	class CommandStorage final
	{
	public:
		/**
		 * Default constructor.
		 */
		CommandStorage() = default;

		/**
		 * Issue a new command to the queue.
		 *
		 * @tparam Args The incoming argument types.
		 * @param arguments The incoming arguments.
		 * @return decltype(auto)
		 */
		template<class... Args>
		decltype(auto) push(Args&&... arguments) { return m_Commands.emplace_back(std::forward<Args>(arguments)...); }

		/**
		 * Get the front command and pop front.
		 *
		 * @return The front command.
		 */
		CommandVariant pop()
		{
			auto command = std::move(m_Commands.front());
			m_Commands.pop_front();

			return command;
		}

		/**
		 * Check if the internal command queue is empty.
		 *
		 * @return Boolean state.
		 */
		[[nodiscard]] bool empty() const noexcept { return m_Commands.empty(); }

		/**
		 * Get the index of a type in the variant.
		 *
		 * @tparam Type The type to get the index of.
		 * @return The index.
		 */
		template<class Type>
		[[nodiscard]] static constexpr uint32_t IndexOf() { return CommandVariantIndex<Type, CommandVariant>::Index; }

	private:
		std::deque<CommandVariant> m_Commands;
	};
}