// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/Client/Controller.hpp"
#include "Engine/Client/Components/Camera.hpp"

class CameraController final : public Flint::Controller
{
public:
	CameraController(Flint::ClientInterface* pClientInterface);

	virtual void OnUpdate(const UI64 delta, const Flint::UpdateSpecification specification) override final;

private:
	void ProcessMouse(Flint::FExtent2D<float> position);

private:
	Flint::GlobalComponentHandle<Flint::Components::Camera> mCameraHandle = {};
	float mMovementBias = 0.5f;
	float mLastX = 0.0f, mLastY = 0.0f;

	bool bFirstMouse = true;
};
