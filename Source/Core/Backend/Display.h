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
		template<class Derived, class InstanceType>
		class Display : public BackendObject<Derived> {
		public:
			using InstanceType = InstanceType;

		public:
			Display() {}
			virtual ~Display() {}

			void Initialize(std::shared_ptr<InstanceType> pInstance, const Vector2 extent, const char* pTitle) { this->pInstance = pInstance, this->mExtent = extent, this->pTitle = pTitle; GetDerived().mInitialize(); }
			void Update() { GetDerived().mUpdate(); }
			void Terminate() { GetDerived().mTerminate(); }

			Inputs::InputCenter* GetInputCenter() const { return const_cast<Inputs::InputCenter*>(&mInputsCenter); }
			Vector2 GetExtent() const { mExtent; }
			const char* GetTitle() const { return pTitle; }

			InstanceType* GetInstance() const { return pInstance.get(); }

			FLINT_SET_NO_COPY_AND_MOVE(Display)
		protected:
			virtual void mInitialize() = 0;
			virtual void mUpdate() = 0;
			virtual void mTerminate() = 0;

			Inputs::InputCenter mInputsCenter = {};
			std::shared_ptr<InstanceType> pInstance = {};

			Vector2 mExtent = Vector2::ZeroAll;
			const char* pTitle = nullptr;
		};
	}
}