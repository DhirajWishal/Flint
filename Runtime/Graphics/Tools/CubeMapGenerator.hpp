// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Device.hpp"

namespace Flint
{
	/**
	 * Flint cube map generator tool.
	 * This tool can be used to convert HDR environment map to cube map.
	 */
	class CubeMapGenerator
	{
	public:
		/**
		 * Construct the object by generating the asset.
		 * 
		 * @param pDevice The device pointer.
		 * @param assetFile The HDR asset file.
		 * @param mips The mip level to use. Default is 1. If the provided count is 0, the default will be set.
		 */
		CubeMapGenerator(Device* pDevice, const std::filesystem::path& assetFile, uint32_t mips = 1);

		/**
		 * Get the generated cube map image.
		 * 
		 * @return The image pointer.
		 */
		std::shared_ptr<Image> GetImage() const { return pCubeMap; }

	private:
		std::shared_ptr<Image> pCubeMap = nullptr;
	};
}
