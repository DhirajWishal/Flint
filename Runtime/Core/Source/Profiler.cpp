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
	Profiler::Profiler(std::string_view functionSignature) : m_FunctionSignature(functionSignature), m_Start(std::chrono::high_resolution_clock::now())
	{
		//OPTICK_EVENT();
	}

	Profiler::~Profiler()
	{
		m_End = std::chrono::high_resolution_clock::now();
		ProfileLogger::GetInstance().WriteContent(*this);
	}

	AtomicProfileControlBlock::AtomicProfileControlBlock(const std::filesystem::path& filePath) : m_ProfileFile(filePath)
	{
		if (!m_ProfileFile.is_open())
			throw std::runtime_error("Failed to open the profile logger!");

		m_ProfileFile << "{\"otherData\": {},\"traceEvents\":[";
		m_ProfileFile.flush();
	}

	AtomicProfileControlBlock::~AtomicProfileControlBlock()
	{
		m_ProfileFile << "]}";
		m_ProfileFile.flush();
		m_ProfileFile.close();
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

	ProfileLogger ProfileLogger::m_Instance;

	void ProfileLogger::WriteContent(const Profiler& profiler)
	{/*
		pControlBlock->Lock();
		uint64_t start = std::chrono::time_point_cast<std::chrono::microseconds>(profiler.m_Start).time_since_epoch().count();
		std::ofstream& file = pControlBlock->m_ProfileFile;

		if (GetInstance().bIsFirst)
			GetInstance().bIsFirst = false;
		else
			file << ",";

		file << "{";
		file << "\"cat\":\"function\",";
		file << "\"dur\":" << (std::chrono::time_point_cast<std::chrono::microseconds>(profiler.m_End).time_since_epoch().count() - start) << ',';
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