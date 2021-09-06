// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "BasicController.hpp"
#include "Engine/Client/ClientInterface.hpp"

BasicController::BasicController(Flint::ClientInterface* pClientInterface)
	: Flint::Controller("BasicController", pClientInterface)
{
	mMaterialHandle = pClientInterface->CreateComponent<Flint::Components::MaterialPipeline>(mIdentifier, "Basic component");
	pClientInterface->CreateComponent<Flint::Components::MaterialPipeline>(mIdentifier, "Secondary");
}
