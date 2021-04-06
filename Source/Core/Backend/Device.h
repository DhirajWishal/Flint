// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Flint device object.
		 * Device objects are the basis of all the assets. This resembles a single (or multiple in the future) GPU(s).
		 * 
		 * @tparam TInstance: The instance type.
		 * @tparam TDisplay: The display type.
		 */
		template<class TInstance, class TDisplay>
		class Device : public BackendObject {
		public:
			using InstanceType = TInstance;
			using DisplayType = TDisplay;

		public:
			Device() {}
			virtual ~Device() {}

			/**
			 * Initialize the device.
			 * 
			 * @param pIstance: The instance object pointer which the device is made to.
			 */
			virtual void Initialize(InstanceType* pInstance) = 0;

			/**
			 * Check if the device and display is compatible.
			 * 
			 * @return Boolean value stating the validity.
			 */
			virtual bool CheckDisplayCompatibility(DisplayType* pDisplay) = 0;

			/**
			 * Terminate the device.
			 */
			virtual void Terminate() = 0;

			/**
			 * Get the instance pointer.
			 * 
			 * @return The instance object pointer.
			 */
			InstanceType* GetInstance() const { return pInstance.get(); }

		protected:
			InstanceType* pInstance = nullptr;
		};
	}
}