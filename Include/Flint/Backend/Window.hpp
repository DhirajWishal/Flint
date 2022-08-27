// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Graphical.hpp"
#include "RenderTarget.hpp"

#include <functional>

namespace Flint
{
	namespace Backend
	{
		/**
		 * Window resize callback function.
		 *
		 * @param The new width of the window.
		 * @param The new height of the window.
		 */
		using WindowResizeCallbackFunction = std::function<void(uint32_t, uint32_t)>;

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
			explicit Window(const std::shared_ptr<Device>& pDevice, std::string&& title, uint32_t width = -1, uint32_t height = -1);

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
			void attach(const std::shared_ptr<RenderTarget>& pRenderTarget, uint32_t attachment = 0);

			/**
			 * Set the resize callback function.
			 * This function is called when a resizing of the window occurs.
			 *
			 * @param function The callback function.
			 */
			void setResizeCallback(WindowResizeCallbackFunction&& function);

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

		protected:
			std::string m_Title;
			std::pair<std::shared_ptr<RenderTarget>, uint32_t> m_Dependency;

			WindowResizeCallbackFunction m_ResizeCallback;

			uint32_t m_Width = 0;
			uint32_t m_Height = 0;
		};
	}
}