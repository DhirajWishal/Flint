// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/Controller.hpp"

#include "Engine/Components/MaterialPipeline.hpp"
#include "Engine/Components/StaticModel.hpp"
#include "Engine/Components/DrawInstance.hpp"
#include "Engine/Components/Uniform.hpp"

class BasicController final : public Flint::Controller
{
public:
	BasicController(Flint::ClientInterface* pClientInterface);

	virtual void OnUpdate(const uint64 delta, const Flint::UpdateSpecification specification) override;

private:
	void SetupMaterialPipeline();
	void SetupStaticModel();
	void SetupDrawInstance();

private:
	Flint::ComponentHandle<Flint::Components::MaterialPipeline> mMaterialHandle = {};
	Flint::ComponentHandle<Flint::Components::StaticModel> mStaticModelHandle = {};
	Flint::ComponentHandle<Flint::Components::DrawInstanceGraphics> mDrawInstance = {};
	Flint::ComponentHandle<Flint::Components::ModelViewProjectionUniform> mUniformMVP = {};

	std::shared_ptr<Flint::DynamicStateContainer> pDynamicStates = nullptr;
	std::shared_ptr<Flint::ResourcePackager> pPackager = nullptr;
	std::shared_ptr<Flint::ResourcePackage> pPackage = nullptr;
};