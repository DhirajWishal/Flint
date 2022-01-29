// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Device.hpp"

#include "Components/EditorRenderTarget.hpp"
#include "Components/ClientInterface/ClientManager.hpp"

namespace Flint
{
	/**
	 * Main application class of the editor.
	 */
	class Application
	{
	public:
		Application();
		~Application();

	private:
		void Execute();

		void HandleClientLoad();

	private:
		void LoadPreviousSession();
		void SaveCurrentSession();

		void SetDefaultsToInterface(ClientInterface* pInterface);

	private:
		std::chrono::steady_clock mSteadyClock = {};

		std::chrono::time_point<std::chrono::steady_clock> mNewTimePoint = {};
		std::chrono::time_point<std::chrono::steady_clock> mOldTimePoint = {};

		std::shared_ptr<Instance> pInstance = nullptr;
		std::shared_ptr<Device> pDevice = nullptr;

		std::unique_ptr<ClientManager> pClientManager = nullptr;

		EditorRenderTarget mRenderTarget;

		float mAverageFrameTime = 0.0f;
		uint64 mFrameCount = 0;

		bool bShouldRun = true;
	};
}