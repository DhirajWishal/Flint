// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Components/Uniform.hpp"
#include "GraphicsCore/Device.hpp"

namespace Flint
{
	namespace Components
	{
		ModelViewProjectionUniform::ModelViewProjectionUniform(const std::shared_ptr<Device>& pDevice)
		{
			pUniformBuffer = pDevice->CreateBuffer(BufferType::Uniform, sizeof(MatrixMVP));
		}

		void ModelViewProjectionUniform::UpdateUniform()
		{
			MatrixMVP* pPayload = static_cast<MatrixMVP*>(pUniformBuffer->MapMemory(sizeof(MatrixMVP)));
			*pPayload = mMatrix;

			pUniformBuffer->UnmapMemory();
		}
	}
}