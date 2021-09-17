// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Application.hpp"
#include "Camera.hpp"

namespace Flint
{
	class GameObject
	{
	public:
		GameObject() = default;

		virtual void Initialize(Application* pApplication) = 0;
		virtual void Update(UI64 delta, Camera* pCamera) = 0;
		virtual void Draw(const std::shared_ptr<CommandBuffer>& pCommandBuffer) = 0;
		virtual void Terminate() = 0;

	protected:
		template<class Type>
		void CopyToBuffer(const std::shared_ptr<Buffer>& pBuffer, const Type& data)
		{
			Type* pData = static_cast<Type*>(pBuffer->MapMemory(sizeof(Type)));
			*pData = data;
			pBuffer->UnmapMemory();
		}
	};
}