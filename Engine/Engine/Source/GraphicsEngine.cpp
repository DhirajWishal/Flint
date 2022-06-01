// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/GraphicsEngine.hpp"
#include "Engine/Errors/InvalidBackend.hpp"

#include "VulkanBackend/VulkanBackend.hpp"

namespace Flint
{
	GraphicsEngine::GraphicsEngine(std::string&& applicationName, uint32_t applicationVersion, bool enableValidation, BackendAPI backendAPI)
	{
		// Setup the create instance command.
		Commands::CreateInstance createInstance;
		createInstance.m_ApplicationName = std::move(applicationName);
		createInstance.m_ApplicationVersion = applicationVersion;
		createInstance.m_EnableValidation = enableValidation;

		// Get the future.
		auto instanceFuture = createInstance.m_Promise.get_future();

		// Create the worker.
		if (backendAPI == BackendAPI::Vulkan)
			m_WorkerThread = std::jthread(VulkanBackend::Worker, std::reference_wrapper(m_ConditionalVariable), std::reference_wrapper(m_ResouceMutex), std::reference_wrapper(m_Commands), std::move(createInstance));
		else
			throw InvalidBackend("The provided backend API is not supported!");

		// Get the instance.
		m_Instance = instanceFuture.get();
	}

	GraphicsEngine::~GraphicsEngine()
	{
		// Issue the termination command.
		issueCommand(Commands::Terminate());

		// Wait till the thread finishes execution.
		m_WorkerThread.join();
	}

	Flint::BackendAPI GraphicsEngine::GetSupportedBackends()
	{
		return BackendAPI::Vulkan;
	}

	void GraphicsEngine::issueCommand(CommandVariant&& variant)
	{
		// Issue the command to the queue.
		{
			auto lock = std::scoped_lock(m_ResouceMutex);
			m_Commands.emplace_back(std::move(variant));
		}

		// Notify that we have a command ready.
		m_ConditionalVariable.notify_one();
	}
}