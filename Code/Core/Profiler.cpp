// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

export module Flint.Core.Profiler;

import <chrono>;
import <fstream>;
import <filesystem>;
import <mutex>;
import <memory>;
import <string>;

export namespace Flint
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
		std::string_view mFunctionSignature = {};
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

module: private;

import <thread>;
import <sstream>;
import <optick.h>;

namespace Flint
{
#ifndef FLINT_RELEASE
	Profiler::Profiler(std::string_view functionSignature) : mFunctionSignature(functionSignature), mStart(std::chrono::high_resolution_clock::now())
	{
		//OPTICK_EVENT();
	}

	Profiler::~Profiler()
	{
		mEnd = std::chrono::high_resolution_clock::now();
		ProfileLogger::GetInstance().WriteContent(*this);
	}

	AtomicProfileControlBlock::AtomicProfileControlBlock(const std::filesystem::path& filePath) : mProfileFile(filePath)
	{
		if (!mProfileFile.is_open())
			throw std::runtime_error("Failed to open the profile logger!");

		mProfileFile << "{\"otherData\": {},\"traceEvents\":[";
		mProfileFile.flush();
	}

	AtomicProfileControlBlock::~AtomicProfileControlBlock()
	{
		mProfileFile << "]}";
		mProfileFile.flush();
		mProfileFile.close();
	}

	ProfileLogger::ProfileLogger()
	{
		std::stringstream threadID;
		threadID << std::this_thread::get_id();
		pControlBlock = std::make_shared<AtomicProfileControlBlock>("ProfileLog.json");
	}

	ProfileLogger::~ProfileLogger()
	{
	}

	ProfileLogger ProfileLogger::mInstance;

	void ProfileLogger::WriteContent(const Profiler& profiler)
	{/*
		pControlBlock->Lock();
		uint64 start = std::chrono::time_point_cast<std::chrono::microseconds>(profiler.mStart).time_since_epoch().count();
		std::ofstream& file = pControlBlock->mProfileFile;

		if (GetInstance().bIsFirst)
			GetInstance().bIsFirst = false;
		else
			file << ",";

		file << "{";
		file << "\"cat\":\"function\",";
		file << "\"dur\":" << (std::chrono::time_point_cast<std::chrono::microseconds>(profiler.mEnd).time_since_epoch().count() - start) << ',';
		file << "\"name\":\"" << profiler.pFunctionSignature << "\",";
		file << "\"ph\":\"X\",";
		file << "\"pid\":0,";
		file << "\"tid\":" << std::this_thread::get_id() << ",";
		file << "\"ts\":" << start;
		file << "}";

		file.flush();
		pControlBlock->Unlock();*/
	}

	void ProfileLogger::SetInstanceContext(const ProfileLogger& other)
	{
		pControlBlock = other.pControlBlock;
		bIsFirst = other.bIsFirst;
	}

#else
	Profiler::Profiler(std::string_view functionSignature)
	{
	}

	Profiler::~Profiler()
	{
	}

#endif // !FLINT_RELEASE

	void NewProfilerFrame()
	{
		OPTICK_FRAME("MainThread");
	}
}