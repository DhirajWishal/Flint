// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SampleScene.hpp"

#include "Controllers/BasicController.hpp"
#include "Controllers/CameraController.hpp"

SampleScene::SampleScene()
{
	mEngine.CreateInstance(true);
	mEngine.CreateNewDevice(Flint::DeviceFlags::External | Flint::DeviceFlags::GraphicsCompatible | Flint::DeviceFlags::ComputeCompatible);
}
