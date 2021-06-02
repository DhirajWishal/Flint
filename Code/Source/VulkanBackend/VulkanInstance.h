// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Instance.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanInstance final : public Instance
		{
		public:
			VulkanInstance(bool enableValidation);

			virtual void Terminate() override final;

		private:
			void InitializeGLFW();
			void TerminateGLFW();
		};
	}
}