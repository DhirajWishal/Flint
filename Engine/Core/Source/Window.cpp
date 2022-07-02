// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Window.hpp"
#include "Core/StaticInitializer.hpp"
#include "Core/Errors/InvalidArgumentError.hpp"

namespace Flint
{
	Window::Window(const std::shared_ptr<Device>& pDevice, std::string&& title, uint32_t width /*= -1*/, uint32_t height /*= -1*/)
		: DeviceBoundObject(pDevice), m_Title(std::move(title)), m_Width(width), m_Height(height)
	{
		// Initialize the static content.
		static StaticInitializer initializer;
	}

	void Window::attach(const std::shared_ptr<RenderTarget>& pRenderTarget, uint32_t attachment /*= 0*/)
	{
		m_Dependency = std::make_pair(pRenderTarget, attachment);
		notifyUpdated();
	}
}