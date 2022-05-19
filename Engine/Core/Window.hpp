// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"

namespace Flint
{
	/**
	 * Window class.
	 * This is a special class, as it contains all the processing stages and finally renders the image to the user using the window created as specified.
	 */
	class Window : public EngineBoundObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param engine The engine to which this window is bound to.
		 * @param title The window title.
		 * @param width The width of the window. If set to 0 then the window is opened maximized. If set to -1, the window is open in full screen mode.
		 * @param height The height of the window. Same as the width, if set to 0 then the window is opened maximized. If set to -1, the window is open in full screen mode.
		 */
		explicit Window(Engine& engine, std::string&& title, uint32_t width = -1, uint32_t height = -1);

		/**
		 * Default virtual destructor.
		 */
		virtual ~Window() = default;

		/**
		 * Update the window.
		 * This will end the previous frame (if it's not the first frame), and begin the next frame. The user can update all the resources after this call.
		 */
		virtual void update() = 0;

		/**
		 * Add a dependency to the window.
		 * This dependency will be copied to the frame.
		 *
		 * @param pRenderTarget The render target dependency.
		 * @param attachment The render target's attachment to copy.
		 */
		void setDependency(const RenderTarget* pRenderTarget, uint32_t attachment);

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

		/**
		 * Get the internal frame count.
		 *
		 * @return The frame count.
		 */
		[[nodiscard]] uint32_t getFrameCount() const { return m_FrameCount; }

	private:
		std::string m_Title;

	protected:
		std::pair<const RenderTarget*, uint32_t> m_Dependency;

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_FrameCount = 0;
	};
}