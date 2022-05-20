// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"
#include "Types.hpp"

#include <vector>

namespace Flint
{
	class Window;
	class Rasterizer;
	class GeometryStore;

	/**
	 * Multisample enum.
	 */
	enum class Multisample : uint8_t
	{
		One,
		Two,
		Four,
		Eight,
		Sixteen,
		ThirtyTwo,
		SixtyFour
	};

	/**
	 * Engine class.
	 * This class contains everything that's needed for a single engine instance.
	 *
	 * Usually an application requires just one instance of this class. But the user isn't restricted to do that in any way. But make sure that the engines are destroyed before
	 * destroying the instance which is used to create this.
	 *
	 * Every engine has a default geometry store. It is advice to use this instead of making a bunch of new ones.
	 */
	class Engine : public FObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance to create the engine with.
		 */
		explicit Engine(Instance& instance) : m_Instance(instance) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~Engine() = default;

		/**
		 * Halt the engine till all the commands and everything else is executed.
		 */
		virtual void waitIdle() = 0;

		/**
		 * Get the best depth pixel format.
		 *
		 * @return The pixel format.
		 */
		[[nodiscard]] virtual PixelFormat getBestDepthFormat() const = 0;

		/**
		 * Get the max supported multisample count.
		 *
		 * @return The multisample count.
		 */
		[[nodiscard]] virtual Multisample getMaximumMultisample() const = 0;

		/**
		 * Get the instance.
		 *
		 * @return The instance reference.
		 */
		[[nodiscard]] Instance& getInstance() { return m_Instance; }

		/**
		 * Get the instance.
		 *
		 * @return The const instance reference.
		 */
		[[nodiscard]] const Instance& getInstance() const { return m_Instance; }

		/**
		 * Get the instance casted to another type.
		 * This is used in inheritance.
		 *
		 * @tparam Type The type to cast to.
		 * @return The type reference.
		 */
		template<class Type>
		[[nodiscard]] Type& getInstanceAs() { return *m_Instance.as<Type>(); }

		/**
		 * Get the instance casted to another type.
		 * This is used in inheritance.
		 *
		 * @tparam Type The type to cast to.
		 * @return The const type reference.
		 */
		template<class Type>
		[[nodiscard]] const Type& getInstanceAs() const { return *m_Instance.as<Type>(); }

		/**
		 * Create a new window.
		 * @ref Window.hpp
		 *
		 * @param title The window title.
		 * @param width The width of the window. Default is -1.
		 * @param height The height of the window. Default is -1.
		 * @return The created window pointer.
		 */
		[[nodiscard]] virtual std::unique_ptr<Window> createWindow(std::string&& title, uint32_t width = -1, uint32_t height = -1) = 0;

		/**
		 * Create a new rasterizer.
		 * @ref Rasterizer.hpp
		 *
		 * @param width The width of the render target.
		 * @param height The height of the render target.
		 * @param frameCount The number of frames in the render target. This is usually set automatically by the Window.
		 * @param attachmentDescriptions The attachment descriptions.
		 * @param multisample The multisample count. Default is One.
		 * @param exclusiveBuffering Whether or not to use one buffer/ attachment per frame. Default is false.
		 * @return The created rasterizer pointer.
		 */
		[[nodiscard]] virtual std::unique_ptr<Rasterizer> createRasterizer(uint32_t width, uint32_t height, uint32_t frameCount, std::vector<AttachmentDescription>&& attachmentDescriptions, Multisample multisample = Multisample::One, bool exclusiveBuffering = false) = 0;

		/**
		 * Create a new geometry store.
		 *
		 * @return The created geometry store.
		 */
		[[nodiscard]] virtual std::unique_ptr<GeometryStore> createGeometryStore() = 0;

		/**
		 * Get the default geometry store.
		 *
		 * @return The geometry store reference.
		 */
		[[nodiscard]] virtual GeometryStore& getDefaultGeometryStore() = 0;

		/**
		 * Get the default geometry store.
		 *
		 * @return The geometry store reference.
		 */
		[[nodiscard]] virtual const GeometryStore& getDefaultGeometryStore() const = 0;

	private:
		Instance& m_Instance;
	};
}