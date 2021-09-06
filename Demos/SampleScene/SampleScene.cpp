// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "SampleScene.hpp"

#include "Controllers/BasicController.hpp"

SampleScene::SampleScene()
{
	RegisterIdentifiers("BasicController");
}

std::shared_ptr<Flint::Controller> SampleScene::CreateController(const std::string_view& identifier)
{
	if (identifier == "BasicController")
		return std::make_shared<BasicController>();

	return std::shared_ptr<Flint::Controller>();
}
