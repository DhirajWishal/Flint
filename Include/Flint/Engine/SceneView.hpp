// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Scene.hpp"

namespace Flint
{
	/**
	 * Scene view class.
	 * This class is used to render a scene.
	 */
	class SceneView : public DeviceBoundObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param pScene The scene to render.
		 */
		explicit SceneView(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Scene>& pScene) : DeviceBoundObject(pDevice), m_pScene(pScene) {}

	protected:
		std::shared_ptr<Scene> m_pScene = nullptr;
	};
}