// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"
#include "Core\Maths\Vector\Vector2.h"
#include "Core\Inputs\InputCenter.h"

namespace Flint
{
	namespace Backend
	{
		template<class InstanceType>
		class Display : public BackendObject {
		public:
			using InstanceType = InstanceType;

		public:
			Display() {}
			virtual ~Display() {}

			virtual void Initialize(InstanceType* pInstance, const Vector2 extent, const char* pTitle) = 0;
			virtual void Update() = 0;
			virtual void Terminate() = 0;

			Inputs::InputCenter* GetInputCenter() const { return const_cast<Inputs::InputCenter*>(&mInputsCenter); }
			Vector2 GetExtent() const { mExtent; }
			const char* GetTitle() const { return pTitle; }

			InstanceType* GetInstance() const { return pInstance; }

			FLINT_SET_NO_COPY_AND_MOVE(Display)
		protected:
			Inputs::InputCenter mInputsCenter = {};
			InstanceType* pInstance = nullptr;

			Vector2 mExtent = Vector2::ZeroAll;
			const char* pTitle = nullptr;
		};
	}
}