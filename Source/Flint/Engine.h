// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/GraphicsPipeline.h"

namespace Flint
{
	enum class BackenAPI : UI8 {
		UNDEFINED,
		VULKAN,
		DIRECT_X_12,
		WEB_GPU
	};

	/**
	 * Flint Engine object.
	 * This is the main object which is responsible of all flint activities.
	 */
	class Engine {
	public:
		Engine() {}
		~Engine() {}

		/**
		 * Initialize the engine.
		 *
		 * @param api: The backend API.
		 * @param enableValidation: Whether or not to enable backend validation.
		 * @param width: The width of the window.
		 * @param heigth: The height of the window.
		 * @param pTitle: The title of the window.
		 */
		void Initialize(BackenAPI api, bool enableValidation, UI32 width, UI32 height, const char* pTitle);

		/**
		 * Terminate the engine.
		 */
		void Terminate();

		/**
		 * Switch from the current backend API to a new API.
		 *
		 * @param newAPI: The new api to be set.
		 */
		void SwitchAPI(BackenAPI newAPI);

		/**
		 * Get the backend API which is currently active.
		 *
		 * @return The backend API type.
		 */
		BackenAPI GetBackendAPI() const { return mAPI; }

	public:
		/**
		 * Create a the internal render target.
		 * The engine uses one internal render target to render everything to. This is based on whether we can use the
		 * device to utilize hardware based ray tracing. If available, we use the hardware accelerated render target, if
		 * not we use the software based render target.
		 *
		 * The buffer count specifies the number of buffers the render target contains. If set to 0, the default
		 * device supported buffer count will be set.
		 * If the count is std::numeric_limits<UI32>::max(), then the maximum supported will be set.
		 *
		 * @param extent: The extent of the render target.
		 * @param bufferCount: The number of buffers to be used. Default is 0.
		 */
		void CreateRenderTarget(const Vector2& extent, UI32 bufferCount = 0);

	private:
		Backend::Instance* pInstance = nullptr;
		Backend::Display* pDisplay = nullptr;
		Backend::Device* pDevice = nullptr;
		Backend::RenderTarget* pRenderTarget = nullptr;

		BackenAPI mAPI = BackenAPI::UNDEFINED;
	};
}