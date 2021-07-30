// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	class Buffer;
	class RenderTarget;

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
		friend RenderTarget;

	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 * @param pipelineName: The name of the pipeline. This name is given to the pipeline cache object created upon destruction. Make sure that this name is unique.
		 *	If you wish to not save cache externally, keep this field empty ("").
		 */
		Pipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName) : DeviceBoundObject(pDevice), mPipelineName(pipelineName) {}

	protected:
		/**
		 * Prepare resources to draw.
		 */
		virtual void PrepareResourcesToDraw() = 0;

		/**
		 * Write the pipeline cache data to an external file.
		 * If the file does not exist, this creates a new file. The file name is "<pipeline name>.fpc". The FPC extension is Flint Pipeline Cache.
		 *
		 * @param size: The size of the data block.
		 * @param pData: The data to be written.
		 */
		void WriteDataToCacheFile(UI64 size, unsigned char* pData) const;

		/**
		 * Read the pipeline cache data from an external file.
		 *
		 * @return The pair of size and data.
		 */
		std::pair<UI64, unsigned char*> ReadDataFromCacheFile() const;

	protected:
		std::string mPipelineName = "";
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