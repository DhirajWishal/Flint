// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DataType.hpp"

namespace Flint
{
	struct ThreadCommandBase {};

	template<class Type>
	struct ThreadCommand : public ThreadCommandBase
	{
		ThreadCommandBase* pNextCommand = nullptr;
	};

	template<UI8 Count>
	class ThreadCommandQueue
	{
	public:
		ThreadCommandQueue() {}


	};
}