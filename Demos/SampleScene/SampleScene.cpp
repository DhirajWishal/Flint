// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SampleScene.hpp"

#include "Controllers/BasicController.hpp"
#include "Controllers/CameraController.hpp"

SampleScene::SampleScene()
{
	RegisterIdentifier("BasicController");
	RegisterIdentifier("CameraController");
}

std::shared_ptr<Flint::Controller> SampleScene::CreateController(const std::string_view& identifier)
{
	if (identifier == "BasicController")
		return std::make_shared<BasicController>(this);

	else if (identifier == "CameraController")
		return std::make_shared<CameraController>(this);

	return std::shared_ptr<Flint::Controller>();
}
