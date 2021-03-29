// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"

namespace Flint
{
	namespace Backend
	{
		template<class Derived, class InstanceType, class DisplayType>
		class Device : public BackendObject<Derived> {
		public:
			using InstanceType = InstanceType;
			using DisplayType = DisplayType;

		public:
			Device() {}
			virtual ~Device() {}

			void Initialize(std::shared_ptr<InstanceType> pInstance) { this->pInstance = pInstance;  GetDerived().mInitialize() }
			bool CheckDisplayCompatibility(DisplayType* pDisplay) {}
			void Terminate() { GetDerived().mTerminate(); }

			InstanceType* GetInstance() const { return pInstance.get(); }

			FLINT_SET_NO_COPY_AND_MOVE(Device)
		protected:
			virtual void mInitialize() = 0;
			virtual bool bCheckDisplayCompatibility(DisplayType* pDisplay) = 0;
			virtual void mTerminate() = 0;

			std::shared_ptr<InstanceType> pInstance = {};
		};
	}
}