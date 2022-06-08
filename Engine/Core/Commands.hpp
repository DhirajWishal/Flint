// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Types.hpp"

#include <deque>
#include <array>
#include <future>
#include <variant>

namespace Flint
{
	FLINT_DEFINE_HANDLE(InstanceHandle);
	FLINT_DEFINE_HANDLE(DeviceHandle);
	FLINT_DEFINE_HANDLE(WindowHandle);
	FLINT_DEFINE_HANDLE(RasterizerHandle);

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
		 * Synchronize command.
		 * This command can be used to synchronize the backend and the parent thread.
		 * Synchronization can also be done using any other command, but this is there for explicitness.
		 */
		struct Synchronize final : public Command<void> {};

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

		/**
		 * Create window command.
		 */
		struct CreateWindow final : public Command<WindowHandle>
		{
			std::string m_Title;

			DeviceHandle m_DeviceHandle;

			uint32_t m_Width = -1;
			uint32_t m_Height = -1;
		};

		/**
		 * Update a window command.
		 */
		struct UpdateWindow final : public Command<void>
		{
			WindowHandle m_WindowHandle;
		};

		/**
		 * Destroy window command.
		 */
		struct DestroyWindow final : public Command<void>
		{
			WindowHandle m_WindowHandle;
		};

		/**
		 * Create rasterizer command.
		 */
		struct CreateRasterizer final : public Command<RasterizerHandle>
		{
			std::vector<Core::AttachmentDescription> m_AttachmentDescriptions;

			DeviceHandle m_DeviceHandle;
			uint32_t m_Width;
			uint32_t m_Height;
			uint32_t m_FrameCount;

			Core::Multisample m_MultisampleCount = Core::Multisample::One;

			bool m_ExclusiveBuffering = false;
		};

		/**
		 * Destroy rasterizer command.
		 */
		struct DestroyRasterizer final : public Command<void>
		{
			RasterizerHandle m_RasterizerHandle;
		};
	}

	/**
	 * Command variant type.
	 * We send commands using variants because it's just easier to do it that way especially that all the commands are in the form of PODs.
	 */
	using CommandVariant = std::variant<
		Commands::Synchronize,
		Commands::Terminate,
		Commands::CreateDevice,
		Commands::DestroyDevice,
		Commands::CreateWindow,
		Commands::UpdateWindow,
		Commands::DestroyWindow,
		Commands::CreateRasterizer,
		Commands::DestroyRasterizer
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