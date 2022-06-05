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

	std::future<Flint::DeviceHandle> GraphicsEngine::createDevice()
	{
		Commands::CreateDevice command;
		auto future = command.m_Promise.get_future();

		issueCommand(std::move(command));
		return future;
	}

	std::future<void> GraphicsEngine::destroyDevice(DeviceHandle handle)
	{
		Commands::DestroyDevice command;
		command.m_DeviceHandle = handle;
		auto future = command.m_Promise.get_future();

		issueCommand(std::move(command));
		return future;
	}

	std::future<Flint::WindowHandle> GraphicsEngine::createWindow(DeviceHandle handle, std::string&& title, uint32_t width /*= -1*/, uint32_t height /*= -1*/)
	{
		Commands::CreateWindow command;
		command.m_DeviceHandle = handle;
		command.m_Title = std::move(title);
		command.m_Width = width;
		command.m_Height = height;
		auto future = command.m_Promise.get_future();

		issueCommand(std::move(command));
		return future;
	}

	std::future<void> GraphicsEngine::updateWindow(WindowHandle handle)
	{
		Commands::UpdateWindow command;
		command.m_WindowHandle = handle;
		auto future = command.m_Promise.get_future();

		issueCommand(std::move(command));
		return future;
	}

	std::future<void> GraphicsEngine::destroyWindow(WindowHandle handle)
	{
		Commands::DestroyWindow command;
		command.m_WindowHandle = handle;
		auto future = command.m_Promise.get_future();

		issueCommand(std::move(command));
		return future;
	}

	void GraphicsEngine::issueCommand(CommandVariant&& variant)
	{
		// Issue the command to the queue.
		{
			auto lock = std::scoped_lock(m_ResouceMutex);
			m_Commands.push(std::move(variant));
		}

		// Notify that we have a command ready.
		m_ConditionalVariable.notify_one();
	}
}