// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DataType.hpp"

namespace Flint
{
	enum class RequestStatus : uint8 {
		Successful,
		Pending,
		Error
	};

	/**
	 * Command base object.
	 * This object is derived by different modules and is used to submit information to the reactor.
	 */
	class CommandBase 
	{
	public:
		CommandBase() = default;
		
		/**
		 * Get the status of the request.
		 * 
		 * @return The status.
		 */
		RequestStatus GetStatus() const { return mStatus; }

	protected:
		std::atomic<RequestStatus> mStatus = RequestStatus::Pending;
	};
}