// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"
#include "Core/Inputs/InputCenter.h"

namespace Flint
{
	namespace Backend
	{
		class Display : public BackendObject {
		public:
			Display() {}

			virtual void Initialize(Instance* pInstance, UI32 width, UI32 height, const char* pTitle = "Flint engine") = 0;
			virtual void Terminate() = 0;

		public:
			Instance* GetInstance() const { return pInstance; }
			Inputs::InputCenter* GetInputCenter() const { return const_cast<Inputs::InputCenter*>(&mInputCenter); }

		protected:
			Instance* pInstance = nullptr;
			Inputs::InputCenter mInputCenter = {};
		};
	}
}