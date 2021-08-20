// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Components/GameObject.hpp"
#include "Components/ImGui.hpp"
#include "Objects/TextureGenerator.hpp"

#include <iostream>

int main(int argc, char** argv)
{
	try
	{
		TextureGenerator generator(nullptr);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}