// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

#include <unordered_map>

namespace Flint
{
	namespace Core
	{
		/**
		 * Program binding table class.
		 * This is used to bind resources to a program. And when drawing, these are used to submit data to the pipelines.
		 */
		template<class TProgram, class TBuffer, class TImageView, class TSampler>
		class ProgramBindingTable
		{
			template<class Type>
			using Container = std::unordered_map<uint32_t, std::unordered_map<uint32_t, Type>>;

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param program The program reference to which this object is bound to.
			 */
			explicit ProgramBindingTable(TProgram& program) : m_Program(program) {}

			/**
			 * Bind a buffer resource to the program.
			 *
			 * @param set The destination set.
			 * @param binding The binding index.
			 * @param buffer The buffer to bind.
			 */
			void bind(uint32_t set, uint32_t binding, const TBuffer& buffer) { m_Buffers[set][binding] = &buffer; }

			/**
			 * Bind an image resource to the program.
			 *
			 * @param set The destination set.
			 * @param binding The binding index.
			 * @param imageView The image view to bind.
			 * @param sampler The image sampler to bind.
			 */
			void bind(uint32_t set, uint32_t binding, const TImageView& imageView, const TSampler& sampler) { m_Images[set][binding] = std::make_pair(&imageView, &sampler); }

		private:
			Container<const TBuffer*> m_Buffers;
			Container<std::pair<const TImageView*, const TSampler*>> m_Images;

			TProgram& m_Program;
		};

		/**
		 * Program class.
		 * Flint uses programs to optimize shaders and resources which are bound to them so that we don't need to create unnecessary resources. They are also used to create pipelines.
		 */
		template<class TDevice>
		class Program : public DeviceBoundObject<TDevice>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device reference.
			 */
			explicit Program(TDevice& device) : DeviceBoundObject<TDevice>(device) {}

			/**
			 * Virtual default destructor.
			 */
			virtual ~Program() = default;
		};
	}
}