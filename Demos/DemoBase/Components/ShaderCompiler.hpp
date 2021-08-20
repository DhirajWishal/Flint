// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <filesystem>

void CompileShader(const std::filesystem::path& shaderFile, const std::filesystem::path& solutionDirectory);
void CompileAllShaders(const std::filesystem::path& solutionDirectory);