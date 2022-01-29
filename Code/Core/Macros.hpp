// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Profiler.hpp"

#define __TO_STRING__(...)						#__VA_ARGS__
#define TO_STRING(...)							__TO_STRING__(__VA_ARGS__)

#define FLINT_RUNTIME_DATA_DIRECTORY			"\\Flint"
#define FLINT_CACHE_DIRECTORY					FLINT_RUNTIME_DATA_DIRECTORY "\\Cache"

#define FLINT_DISABLE_COPY(className)			className(const className&) = delete; className& operator=(const className&) = delete
#define FLINT_DISABLE_MOVE(className)			className(className&&) = delete; className& operator=(className&&) = delete
