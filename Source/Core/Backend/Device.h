// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"

namespace Flint
{
	namespace Backend
	{
		template<class TInstance, class TDisplay>
		class Device : public BackendObject {
		public:
			using InstanceType = TInstance;
			using DisplayType = TDisplay;

		public:
			Device() {}
			virtual ~Device() {}

			virtual void Initialize(InstanceType* pInstance) = 0;
			virtual bool CheckDisplayCompatibility(DisplayType* pDisplay) = 0;
			virtual void Terminate() = 0;

			InstanceType* GetInstance() const { return pInstance.get(); }

			FLINT_SET_NO_COPY_AND_MOVE(Device)
		protected:
			InstanceType* pInstance = nullptr;
		};
	}
}