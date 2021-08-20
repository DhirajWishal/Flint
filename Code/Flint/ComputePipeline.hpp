// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"
#include "Shader.hpp"
#include "RenderTarget.hpp"
#include "DynamicStateContainer.hpp"
#include "ResourceMap.hpp"

namespace Flint
{
	class ScreenBoundRenderTarget;
	class OffScreenRenderTarget;

	/**
	 * Compute dispatch mode.
	 * This will define when to dispatch the compute instances when attached to a render target.
	 */
	enum class ComputeDispatchMode : UI8 {
		BEFORE_EXECUTION,
		AFTER_EXECUTION,

		BEFORE_EVERY_EXECUTION,
		AFTER_EVERY_EXECUTION,
	};

	/**
	 * Flint compute pipeline.
	 * This pipeline is used to perform compute tasks on the GPU.
	 */
	class ComputePipeline : public Pipeline
	{
		struct ComputeInstance
		{
			ComputeInstance() = default;
			ComputeInstance(const std::shared_ptr<ResourceMap>& pResourceMap, const std::shared_ptr<DynamicStateContainer>& pDynamicStates, const FBox3D& computeGroups)
				: pResourceMap(pResourceMap), pDynamicStates(pDynamicStates), mComputeGroups(computeGroups) {}

			std::shared_ptr<ResourceMap> pResourceMap = nullptr;
			std::shared_ptr<DynamicStateContainer> pDynamicStates = nullptr;

			FBox3D mComputeGroups = {};
		};

	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 * @param pipelineName: The name of the pipeline.
		 * @param pComputeShader: The compute shader pointer.
		 */
		ComputePipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName, const std::shared_ptr<Shader>& pComputeShader);

		/**
		 * Dispatch compute commands to the GPU.
		 * This will dispatch all commands to the GPU at once.
		 */
		virtual void Dispatch() = 0;

		/**
		 * Dispatch compute commands to the GPU.
		 * This will be executed depending on the dispatch mode.
		 * 
		 * @param pScreenBoundRenderTarget: The screen bound render target pointer.
		 */
		virtual void Dispatch(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget) = 0;

		/**
		 * Dispatch compute commands to the GPU.
		 * This will be executed depending on the dispatch mode.
		 *
		 * @param pOffScreenRenderTarget: The off screen render target pointer.
		 */
		virtual void Dispatch(const std::shared_ptr<OffScreenRenderTarget>& pOffScreenRenderTarget) = 0;

	public:
		/**
		 * Add a new compute instance to the pipeline.
		 * 
		 * @param pResourceMap: The resource map of the instance.
		 * @param pDynamicStates: The dynamic states pointer.
		 * @param computeGroups: The compute groups.
		 * @return The instance ID.
		 */
		const UI64 AddInstance(const std::shared_ptr<ResourceMap>& pResourceMap, const std::shared_ptr<DynamicStateContainer>& pDynamicStates, const FBox3D& computeGroups);

		/**
		 * Remove a compute instance form the pipeline.
		 * 
		 * @param ID: The compute instance ID.
		 */
		void RemoveInstance(const UI64 ID);

	protected:
		std::shared_ptr<Shader> pShader = nullptr;
		std::unordered_map<UI64, ComputeInstance> mComputeInstances = {};

		UI64 mInstanceIndex = 0;
	};
}