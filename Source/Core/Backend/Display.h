// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"

#include "Core\Inputs\InputCenter.h"
#include "Core\Maths\Vector\Vector2.h"

namespace Flint
{
	namespace Backend
	{
		class Display : public BackendObject
		{
		public:
			Display(Instance* pInstance, const Vector2 extent, const char* pTitle) : pInstance(pInstance), mExtent(extent), pTitle(pTitle) {}
			virtual ~Display() {}

			virtual void Update() = 0;

			Inputs::InputCenter* GetInputCenter() const { return const_cast<Inputs::InputCenter*>(&mInputsCenter); }
			Vector2 GetExtent() const { return mExtent; }
			const char* GetTitle() const { return pTitle; }
			Instance* GetInstance() const { return pInstance; }

		protected:
			Inputs::InputCenter mInputsCenter = {};
			Instance* pInstance = nullptr;

			Vector2 mExtent = Vector2::Zero;
			const char* pTitle = nullptr;
		};
	}
}