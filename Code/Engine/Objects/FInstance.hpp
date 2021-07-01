// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"

namespace Flint
{
	/**
	 * Backend API enum.
	 */
	enum class BackendAPI {
		VULKAN,
		DIRECTX_12
	};

	/**
	 * Flint instance object.
	 * This object contains a single instance backend object.
	 */
	class FInstance
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param api: The backend API to use.
		 * @param enableValidation: Whether or not to enable backend validation.
		 */
		FInstance(BackendAPI api, bool enableValidation);

		/**
		 * Default destructor.
		 */
		~FInstance();

		/**
		 * Get the backend API used.
		 *
		 * @return The API type.
		 */
		const BackendAPI GetAPI() const { return mAPI; }

		/**
		 * Get the primitive backend object.
		 * 
		 * @return The instance object reference.
		 */
		Instance& GetPrimitive() { return mInstance; }

		/**
		 * Get the primitive backend object.
		 *
		 * @return The instance object reference.
		 */
		const Instance& GetPrimitive() const { return mInstance; }

	public:
		/**
		 * Arrow operator overload.
		 *
		 * @return The backend instance object reference.
		 */
		Instance& operator->() { return mInstance; }

		/**
		 * Arrow operator overload.
		 *
		 * @return The backend instance object reference.
		 */
		const Instance& operator->() const { return mInstance; }

		/**
		 * Backend instance object reference operator
		 *
		 * @return The backend instance object reference.
		 */
		operator Instance& () { return mInstance; }

		/**
		 * Backend instance object reference operator
		 *
		 * @return The backend instance object reference.
		 */
		operator const Instance& () const { return mInstance; }

	protected:
		Instance& mInstance;
		BackendAPI mAPI = BackendAPI::VULKAN;
	};
}
