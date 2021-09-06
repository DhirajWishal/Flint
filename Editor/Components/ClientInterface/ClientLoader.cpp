// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ClientLoader.hpp"

#ifdef FLINT_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#endif

namespace Flint
{
	ClientLoader::ClientLoader(const std::filesystem::path& executable)
	{
#ifdef FLINT_PLATFORM_WINDOWS
		if (executable.extension().string() != ".dll")
			throw std::invalid_argument("The provided file is not a valid windows executable!");

		HMODULE pClientModule = LoadLibrary(executable.c_str());

		if (!pClientModule)
			throw std::runtime_error("Failed to load the client executable!");

		mCreateClientFunction = reinterpret_cast<TCreateClientFunction>(GetProcAddress(pClientModule, "CreateClient"));
		mDestroyClientFunction = reinterpret_cast<TDestroyClientFunction>(GetProcAddress(pClientModule, "DestroyClient"));
		pModuleHandle = pClientModule;

#endif

		if (!mCreateClientFunction || !mDestroyClientFunction)
			throw std::runtime_error("Could not load the client symbols!");

		pClientInterface = mCreateClientFunction();
		InitializeClient();
	}
	
	ClientLoader::~ClientLoader()
	{
#ifdef FLINT_PLATFORM_WINDOWS
		mDestroyClientFunction(pClientInterface);
		FreeLibrary(static_cast<HMODULE>(pModuleHandle));

#endif
	}
	
	void ClientLoader::InitializeClient()
	{
		pClientInterface->ActivateController(pClientInterface->GetClassIdentifiers().front());
	}
}