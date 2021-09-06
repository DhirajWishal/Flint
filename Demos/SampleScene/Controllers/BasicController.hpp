// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/Client/Controller.hpp"

class BasicController final : public Flint::Controller
{
public:
	BasicController() : Flint::Controller("BasicController") {}
};