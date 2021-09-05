// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Device.hpp"

#include "Components/EditorRenderTarget.hpp"

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

	private:
		std::chrono::steady_clock mSteadyClock = {};
		std::chrono::time_point<std::chrono::steady_clock> mNewTimePoint = {};
		std::chrono::time_point<std::chrono::steady_clock> mOldTimePoint = {};

		std::shared_ptr<Instance> pInstance = nullptr;
		std::shared_ptr<Device> pDevice = nullptr;

		EditorRenderTarget mRenderTarget;

		float mAverageFrameTime = 0.0f;
		UI64 mFrameCount = 0;

		bool bShouldRun = true;
	};
}