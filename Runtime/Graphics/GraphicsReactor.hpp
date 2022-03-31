// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <thread>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <deque>

#include "GraphicsCore/Device.hpp"

namespace Flint
{
	class GraphicsEngine;

	/**
	 * Flint graphics reactor object.
	 */
	class GraphicsReactor
	{
		using CommandFunction = std::function<void()>;

	protected:
		/**
		 * Constructor.
		 * 
		 * @param pEngine The engine to which this object is bound to.
		 * @param flags The required device flags.
		 */
		explicit GraphicsReactor(GraphicsEngine* pEngine, DeviceFlags flags = DeviceFlags::External | DeviceFlags::GraphicsCompatible | DeviceFlags::ComputeCompatible);

		/**
		 * Virtual destructor.
		 * This will automatically terminate the device.
		 */
		virtual ~GraphicsReactor();

		/**
		 * Worker function.
		 * 
		 * @param pEngine The engine to which this reactor is bound to.
		 * @param flags The required device flags.
		 */
		virtual void Worker(GraphicsEngine* pEngine, DeviceFlags flags) = 0;

	public:
		/**
		 * Issue a new command to the reactor.
		 * 
		 * @param function The command function.
		 */
		void issueCommand(const CommandFunction& function);

		/**
		 * Issue a new command to the reactor.
		 *
		 * @param function The command function.
		 */
		void issueCommand(CommandFunction&& function);

	protected:
		std::jthread m_WorkerThread;
		std::condition_variable m_ConditionalVariable;
		std::mutex m_Mutex;
		std::deque<CommandFunction> m_Commands;
		bool m_bShouldStop = false;
	};
}