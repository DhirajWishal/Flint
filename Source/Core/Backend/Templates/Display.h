// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Inputs/InputCenter.h"
#include "Core/Maths/Vector/Vector2.h"

namespace Flint
{
	namespace Templates
	{
		/**
		 * Flint display template.
		 */
		class Display
		{
		public:
			Display(const Vector2& extent, const std::string& title) : mExtent(extent), mTitle(title) {}

			virtual void Update() = 0;

			Inputs::InputCenter* GetInputCenter() { return &mInputCenter; }
			const Inputs::InputCenter* GetInputCenter() const { return &mInputCenter; }

			const Vector2 GetExtent() const { return mExtent; }

		protected:
			Inputs::InputCenter mInputCenter = {};
			std::string mTitle = "";
			Vector2 mExtent = {};
		};
	}
}