// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Texture2D.hpp"
#include "Core/Errors/InvalidArgumentError.hpp"

#include <unordered_map>

namespace Flint
{
	/**
	 * Asset registry class.
	 * This essentially maps a string to an asset (buffers, textures and shaders).
	 */
	class AssetRegistry final
	{
	public:
		/**
		 * Default constructor.
		 */
		AssetRegistry() = default;

		/**
		 * Register a new asset object.
		 *
		 * @tparam Type The type of the asset.
		 * @param identifier The asset identifier.
		 * @param pAsset The asset pointer to register.
		 */
		template<class Type>
		void registerAsset(const std::string& identifier, const std::shared_ptr<Type>& pAsset)
		{
			// If the type is Texture2D, save it in its container.
			if constexpr (std::is_same_v<Type, Texture2D>)
				m_Texture2Ds[identifier] = pAsset;

			// Else if the type is Buffer, save it in its container.
			else if constexpr (std::is_same_v<Type, Buffer>)
				m_Buffers[identifier] = pAsset;
		}

		/**
		 * Get a stored asset from the registry.
		 *
		 * @tparam Type The type of the asset.
		 * @param identifier The asset's identifier.
		 * @return The asset pointer.
		 */
		template<class Type>
		[[nodiscard]] std::shared_ptr<Type> getAsset(const std::string& identifier)
		{
			// If the type is Texture2D, save it in its container.
			if constexpr (std::is_same_v<Type, Texture2D>)
				return m_Texture2Ds.at(identifier);

			// Else if the type is Buffer, save it in its container.
			else if constexpr (std::is_same_v<Type, Buffer>)
				return m_Buffers.at(identifier);

			else
				throw InvalidArgumentError("The provided template argument is invalid!");
		}

		/**
		 * Get a stored asset from the registry.
		 *
		 * @tparam Type The type of the asset.
		 * @param identifier The asset's identifier.
		 * @return The asset pointer.
		 */
		template<class Type>
		[[nodiscard]] const std::shared_ptr<Type> getAsset(const std::string& identifier) const
		{
			// If the type is Texture2D, save it in its container.
			if constexpr (std::is_same_v<Type, Texture2D>)
				return m_Texture2Ds.at(identifier);

			// Else if the type is Buffer, save it in its container.
			else if constexpr (std::is_same_v<Type, Buffer>)
				return m_Buffers.at(identifier);

			else
				throw InvalidArgumentError("The provided template argument is invalid!");
		}

		/**
		 * Check if an asset is registered or not.
		 *
		 * @tparam Type The type of the asset.
		 * @param identifier The asset's identifier.
		 * @return Boolean value stating if it's registered or not.
		 */
		template<class Type>
		[[nodiscard]] bool isRegistered(const std::string& identifier) const
		{
			// If the type is Texture2D, save it in its container.
			if constexpr (std::is_same_v<Type, Texture2D>)
				return m_Texture2Ds.contains(identifier);

			// Else if the type is Buffer, save it in its container.
			else if constexpr (std::is_same_v<Type, Buffer>)
				return m_Buffers.contains(identifier);

			else
				throw InvalidArgumentError("The provided template argument is invalid!");
		}

	private:
		std::unordered_map<std::string, std::shared_ptr<Buffer>> m_Buffers;
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_Texture2Ds;
	};
}