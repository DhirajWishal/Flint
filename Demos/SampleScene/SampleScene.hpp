// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/ClientInterface.hpp"

class SampleScene final : public Flint::ClientInterface
{
public:
	SampleScene();

	virtual std::shared_ptr<Flint::Controller> CreateController(const std::string_view& identifier) override final;
};