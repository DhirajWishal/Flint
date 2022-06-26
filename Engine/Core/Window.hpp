// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Graphical.hpp"
#include "RenderTarget.hpp"
#include "StaticInitializer.hpp"

namespace Flint
{
	/**
	 * Window class.
	 * This is a special class, as it contains all the processing stages and finally renders the image to the user using the window created as specified.
	 */
	class Window : public DeviceBoundObject, public Graphical
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device to which this window is bound to.
		 * @param title The window title.
		 * @param width The width of the window. If set to 0 then the window is opened maximized. If set to -1, the window is open in full screen mode.
		 * @param height The height of the window. Same as the width, if set to 0 then the window is opened maximized. If set to -1, the window is open in full screen mode.
		 */
		explicit Window(const std::shared_ptr<Device>& pDevice, std::string&& title, uint32_t width = -1, uint32_t height = -1)
			: DeviceBoundObject(pDevice), m_Title(std::move(title)), m_Width(width), m_Height(height)
		{
			// Initialize the static content.
			static StaticInitializer initializer;
		}

		/**
		 * Default virtual destructor.
		 */
		virtual ~Window() = default;

		/**
		 * Attach a dependency to the window.
		 * The contents of the dependency will be copied to the window when updating.
		 *
		 * If the render target attachment's size is not equal to the window, then it will scale it down accordingly.
		 *
		 * @param pRenderTarget The render target dependency.
		 * @param attachment The render target's attachment to copy. Default is 0.
		 */
		void attach(const std::shared_ptr<RenderTarget>& pRenderTarget, uint32_t attachment = 0) { m_Dependency = std::make_pair(pRenderTarget, attachment); notifyUpdated(); }

		/**
		 * Get the title of the window.
		 *
		 * @return The window title.
		 */
		[[nodiscard]] std::string_view getTitle() const { return m_Title; }

		/**
		 * Get the width of the window.
		 *
		 * @return The width.
		 */
		[[nodiscard]] uint32_t getWidth() const { return m_Width; }

		/**
		 * Get the height of the window.
		 *
		 * @return The height.
		 */
		[[nodiscard]] uint32_t getHeight() const { return m_Height; }

	private:
		std::string m_Title;

	protected:
		std::pair<std::shared_ptr<RenderTarget>, uint32_t> m_Dependency;

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};
}