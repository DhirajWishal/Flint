// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Parents/WireFrameManager.h"
#include "Components/SceneComponent.h"

namespace Flint
{
	/**
	 * Flint Engine object.
	 * This is the main object which is responsible of all flint activities.
	 */
	class Engine : public WireFrameManager {
	public:
		Engine() {}
		~Engine() {}

		/**
		 * Initialize the engine.
		 *
		 * @param api: The backend API.
		 * @param enableValidation: Whether or not to enable backend validation.
		 * @param width: The width of the window.
		 * @param height: The height of the window.
		 * @param pTitle: The title of the window.
		 */
		void Initialize(BackendAPI api, bool enableValidation, UI32 width, UI32 height, const char* pTitle);

		/**
		 * Terminate the engine.
		 */
		void Terminate();

		/**
		 * Switch from the current backend API to a new API.
		 *
		 * @param newAPI: The new api to be set.
		 */
		void SwitchAPI(BackendAPI newAPI);

		/**
		 * Get the backend API which is currently active.
		 *
		 * @return The backend API type.
		 */
		BackendAPI GetBackendAPI() const { return mAPI; }

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

	public:
		/**
		 * Create a new scene component based on the graphics pipeline.
		 * 
		 * @param wireFrame: The wire frame to be used.
		 * @param shaders: The shaders to be used.
		 * @param spec: The graphics pipeline spec.
		 * @return The newly created scene component object.
		 */
		SceneComponent CreateSceneComponent(const WireFrame& wireFrame, const std::vector<ShaderDigest>& shaders, const Backend::GraphicsPipelineSpecification& spec);

		/**
		 * Destroy a created scene component object.
		 * 
		 * @param sceneComponent: The scene component to be destroyed.
		 */
		void DestroySceneComponent(SceneComponent& sceneComponent);

	private:
		Backend::RenderTarget* pRenderTarget = nullptr;
	};
}