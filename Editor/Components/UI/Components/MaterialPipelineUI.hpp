// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/ComponentSystem/ComponentRegistry.hpp"
#include "Engine/Components/MaterialPipeline.hpp"

namespace Flint
{
	class MaterialPipelineUI
	{
	public:
		MaterialPipelineUI(ComponentIdentifier& identifier, const Components::MaterialPipeline& pipeline);
		~MaterialPipelineUI();

	private:
		void DisplayShader(const std::shared_ptr<Shader>& pShader, const std::string& title);
	};
}
