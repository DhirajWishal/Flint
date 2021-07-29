// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	class Buffer;

	/**
	 * Resource descriptor structure.
	 * This structure defines how a set of resources should be bound to a pipeline.
	 */
	struct ResourceDescriptor
	{
		/**
		 * Buffer attachment structure.
		 * This structure contains a buffer's attachment information.
		 */
		struct BufferAttachment
		{
			BufferAttachment() = default;
			BufferAttachment(const std::string& resourceName, const std::shared_ptr<Buffer>& pBuffer) : mResourceName(resourceName), pBuffer(pBuffer) {}

			std::string mResourceName;
			std::shared_ptr<Buffer> pBuffer = nullptr;
		};

	public:
		ResourceDescriptor() = default;

		std::vector<BufferAttachment> pBufferAttachments;
	};

	/**
	 * Flint pipeline object.
	 * This object is used to store information about a rendering or compute pipeline.
	 */
	class Pipeline : public DeviceBoundObject
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 */
		Pipeline(const std::shared_ptr<Device>& pDevice) : DeviceBoundObject(pDevice) {}
	};

	namespace Helpers
	{
		/**
		 * Create a new buffer resource attachment object.
		 *
		 * @param resourceName: The name of the resource to be bound to.
		 * @param pBuffer: The buffer pointer.
		 * @return The newly created Buffer Attachment object.
		 */
		ResourceDescriptor::BufferAttachment CreateBufferResourceAttachment(const std::string& resourceName, const std::shared_ptr<Buffer>& pBuffer);
	}
}