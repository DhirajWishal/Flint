// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

/**
 * Package objects in the Flint::Objects namespace along with some utility functions.
 */
#define FLINT_PACK_OBJECTS(																																						\
	instance,																																									\
	device,																																										\
	display,																																									\
	buffer,																																										\
	image,																																										\
	renderTargetSB,																																								\
	graphicsPipeline,																																							\
	pipelineResource,																																							\
	commandBufferList)																																							\
																																												\
	namespace Flint																																								\
	{																																											\
		/*																																										\
		 * Objects namespace contains object type definitions which are common for all the backend APIs. In other words, these object typedefs will be common for all the 		\
		 * backend APIs and provides object type abstraction.																													\
		 * This also contains some utility functions to perform some tasks like creating objects.																				\
		 */																																										\
		namespace Objects																																						\
		{																																										\
			typedef instance Instance;																																			\
			typedef device Device;																																				\
			typedef display Display;																																			\
			typedef buffer Buffer;																																				\
			typedef image Image;																																				\
			typedef renderTargetSB ScreenBoundRenderTarget;																														\
			typedef commandBufferList CommandBufferList;																														\
																																												\
			typedef graphicsPipeline<ScreenBoundRenderTarget> ScreenBoundGraphicsPipeline;																						\
			typedef pipelineResource<ScreenBoundGraphicsPipeline> ScreenBoundGraphicsPipelineResource;																			\
																																												\
			typedef Backend::UniformBufferContainer<Buffer>	UniformBufferContainer;																								\
			typedef Backend::UniformBufferContainer<Image>	UniformImageContainer;																								\
																																												\
			/*																																									\
			 * Create a new buffer object.																																		\
			 *																																									\
			 * @param pDevice: The device pointer.																																\
			 * @param size: The size of the buffer.																																\
			 * @param usage: The buffer usage.																																	\
			 * @param profile: The buffer's memory profile.																														\
			 * @return The newly created buffer object.																															\
			 */																																									\
			inline Buffer CreateBuffer(Device* pDevice, UI64 size, Backend::BufferUsage usage, Backend::MemoryProfile profile)													\
			{																																									\
				Buffer mBuffer = {};																																			\
				mBuffer.Initialize(pDevice, size, usage, profile);																												\
				return mBuffer;																																					\
			}																																									\
		}																																										\
	}