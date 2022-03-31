// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Graphics/GraphicsReactor.hpp"

namespace Flint
{
	GraphicsReactor::GraphicsReactor(GraphicsEngine* pEngine, DeviceFlags flags)
		: m_WorkerThread(this, &GraphicsReactor::Worker, pEngine, flags)
	{
	}

	GraphicsReactor::~GraphicsReactor()
	{
		// If the thread is not join-able, we can wait till it finished executing all the remaining commands.
		if (!m_WorkerThread.joinable())
		{
			auto lock = std::scoped_lock(m_Mutex);
			m_bShouldStop = true;
		}

		m_ConditionalVariable.notify_one();
		m_WorkerThread.join();
	}

	void GraphicsReactor::issueCommand(const CommandFunction& function)
	{
		// Issue the command to the queue.
		{
			auto lock = std::scoped_lock(m_Mutex);
			m_Commands.push(function);
		}

		// Notify that we have a command ready.
		m_ConditionalVariable.notify_one();
	}

	void GraphicsReactor::issueCommand(CommandFunction&& function)
	{
		// Issue the command to the queue.
		{
			auto lock = std::scoped_lock(m_Mutex);
			m_Commands.push(std::move(function));
		}

		// Notify that we have a command ready.
		m_ConditionalVariable.notify_one();
	}
}