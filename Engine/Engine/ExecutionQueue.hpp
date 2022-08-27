// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Graphical.hpp"
#include "Core/CommandBuffers.hpp"

namespace Flint
{
	/**
	 * Execution queue class.
	 * This class contains a list of graphical objects which are to be executed.
	 *
	 * Graphical objects can be queued and the objects will be executed/ submitted to the GPU in the given order. Note that the required objects needs to be
	 * updated externally.
	 */
	class ExecutionQueue final : public Backend::DeviceBoundObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device to which this object belongs.
		 */
		explicit ExecutionQueue(const std::shared_ptr<Backend::Device>& pDevice);

		/**
		 * Default destructor.
		 */
		~ExecutionQueue() override = default;

		/**
		 * Add a graphical object to the pipeline.
		 *
		 * @param pGraphicalObject The graphical object to be inserted.
		 */
		void insert(const std::shared_ptr<Backend::Graphical>& pGraphicalObject) { m_pGraphicalObjects.emplace_back(pGraphicalObject); }

		/**
		 * Submit the backend command buffers to the GPU and execute them.
		 */
		void execute();

	private:
		std::vector<std::shared_ptr<Backend::Graphical>> m_pGraphicalObjects;
		std::shared_ptr<Backend::CommandBuffers> m_pCommandBuffers;
	};
}