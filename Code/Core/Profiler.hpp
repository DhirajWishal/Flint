// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <chrono>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <memory>
#include <string>

namespace Flint
{
	class ProfileLogger;

	/**
	 * Flint profiler class.
	 * This object is used to store time stamp and function information while profiling.
	 *
	 * This counts the starting time using the constructor and logs it to the profiler upon its destruction.
	 * The object functions only in debug and pre release modes.
	 */
	class Profiler
	{
		friend ProfileLogger;

	public:
		Profiler(std::string_view functionSignature);
		~Profiler();

	private:
#ifndef FLINT_RELEASE
		std::string_view mFunctionSignature = nullptr;
		std::chrono::time_point<std::chrono::high_resolution_clock> mStart = {};
		std::chrono::time_point<std::chrono::high_resolution_clock> mEnd = {};

#endif // !FLINT_RELEASE
	};

#ifndef FLINT_RELEASE
	/**
	 * Atomic profile control block structure.
	 * This contains the output file and the mutex which is used in multi threading.
	 */
	struct AtomicProfileControlBlock
	{
		AtomicProfileControlBlock(const std::filesystem::path& filePath);
		~AtomicProfileControlBlock();

		/**
		 * Lock the resources.
		 */
		void Lock() { while (!mMutex.try_lock()); }

		/**
		 * Unlock the resources.
		 */
		void Unlock() { mMutex.unlock(); }

		std::ofstream mProfileFile = {};
		std::mutex mMutex = {};
	};

	/**
	 * Flint profile logger class.
	 * This object is used to store information to the logger file. The file type is json.
	 *
	 * This object can only be used in debug and pre release modes.
	 */
	class ProfileLogger
	{
		ProfileLogger();
		~ProfileLogger();
		static ProfileLogger mInstance;

	public:
		/**
		 * Get the current profiler instance.
		 *
		 * @return the instance reference.
		 */
		static ProfileLogger& GetInstance() { return mInstance; }

		/**
		 * Write a profile content to the file.
		 *
		 * @param profiler The profiler object.
		 */
		void WriteContent(const Profiler& profiler);

		/**
		 * Set profile context from another profiler to this.
		 *
		 * @param other The other profile logger.
		 */
		void SetInstanceContext(const ProfileLogger& other);

	private:
		std::shared_ptr<AtomicProfileControlBlock> pControlBlock = nullptr;
		bool bIsFirst = true;
	};

#endif // !FLINT_RELEASE

	/**
	 * Set a new profiler frame.
	 * This should be done once per frame if optick is to be used.
	 */
	void NewProfilerFrame();
}
