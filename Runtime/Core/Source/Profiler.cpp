// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Profiler.hpp"
#include "Core/Error.hpp"

#include <thread>
#include <sstream>

#include <optick.h>

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
		uint64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(profiler.mStart).time_since_epoch().count();
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