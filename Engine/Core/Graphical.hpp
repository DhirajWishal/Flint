// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>

namespace Flint
{
	/**
	 * Graphical class.
	 * This class is the base class for all the graphical objects which supports graphics (rasterizing or ray tracing).
	 */
	class Graphical
	{
	public:
		/**
		 * Default constructor.
		 */
		constexpr Graphical() = default;

		/**
		 * Explicit constructor.
		 *
		 * @param frameCount The number of frames which are been updated by the graphical object.
		 */
		explicit Graphical(uint32_t frameCount);

		/**
		 * Default virtual destructor.
		 */
		virtual ~Graphical() = default;

		/**
		 * Update the object.
		 * This is where all the graphics are submitted to the GPU.
		 */
		virtual void update() = 0;

		/**
		 * Get the internal frame count.
		 *
		 * @return The frame count.
		 */
		[[nodiscard]] uint32_t getFrameCount() const { return m_FrameCount; }

		/**
		 * Get the current frame index.
		 *
		 * @return The frame index.
		 */
		[[nodsicard]] uint32_t getFrameIndex() const { return m_FrameIndex; }

		/**
		 * Notify that everything needs to be updated once more.
		 */
		void toggleNeedToUpdate() { m_NeedToUpdate = m_FrameCount; }

	protected:
		/**
		 * Notify a single frame is updated.
		 */
		void notifyUpdated() { --m_NeedToUpdate; }

		/**
		 * Check if we need to update the command buffers again.
		 *
		 * @return Whether or not to update everything again.
		 */
		[[nodiscard]] bool needToUpdate() const { return m_NeedToUpdate > 0; }

		/**
		 * Increment the frame index to the next one.
		 */
		void incrementFrameIndex() { m_FrameIndex = ++m_FrameIndex % m_FrameCount; }

	protected:
		uint32_t m_FrameCount = 0;

		uint32_t m_NeedToUpdate = 0;	// This variable is set the frame count if the command buffers need to be updated again.
		uint32_t m_FrameIndex = 0;
	};
}