// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/Controller.hpp"
#include "Engine/Components/Camera.hpp"

class CameraController final : public Flint::Controller
{
public:
	CameraController(Flint::ClientInterface* pClientInterface);

	virtual void OnUpdate(const uint64 delta, const Flint::UpdateSpecification specification) override;

private:
	void ProcessMouse(Flint::FExtent2D<float> position);

private:
	Flint::ComponentHandle<Flint::Components::Camera> mCameraHandle = {};
	float mMovementBias = 0.5f;
	float mLastX = 0.0f, mLastY = 0.0f;

	bool bFirstMouse = true;
};
