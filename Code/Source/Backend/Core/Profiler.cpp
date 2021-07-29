// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Profiler.hpp"
#include "Core/Error.hpp"

#include <thread>
#include <sstream>

namespace Flint
{
#ifndef FLINT_RELEASE
	Profiler::Profiler(const char* pFunctionSignature) : pFunctionSignature(pFunctionSignature), mStart(std::chrono::high_resolution_clock::now())
	{
	}

	Profiler::~Profiler()
	{
		mEnd = std::chrono::high_resolution_clock::now();
		ProfileLogger::GetInstance().WriteContent(*this);
	}

	AtomicProfileControlBlock::AtomicProfileControlBlock(const std::filesystem::path& filePath) : mProfileFile(filePath)
	{
		if (!mProfileFile.is_open())
			FLINT_THROW_RUNTIME_ERROR("Failed to open the profile logger!");

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
		pControlBlock = std::make_shared<AtomicProfileControlBlock>("ProfileLog_" + threadID.str() + ".json");
	}

	ProfileLogger::~ProfileLogger()
	{
	}

	ProfileLogger ProfileLogger::mInstance;

	void ProfileLogger::WriteContent(const Profiler& profiler)
	{
		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(profiler.mStart).time_since_epoch().count();
		pControlBlock->Lock();
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
		pControlBlock->Unlock();
	}

	void ProfileLogger::SetInstanceContext(const ProfileLogger& other)
	{
		pControlBlock = other.pControlBlock;
		bIsFirst = other.bIsFirst;
	}

#else
	Profiler::Profiler(const char* pFunctionSignature)
	{
	}

	Profiler::~Profiler()
	{
	}

#endif // !FLINT_RELEASE
}