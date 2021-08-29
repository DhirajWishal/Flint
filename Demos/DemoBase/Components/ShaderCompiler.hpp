// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <filesystem>

void CompileShader(const boost::filesystem::path& shaderFile, const boost::filesystem::path& solutionDirectory);
void CompileAllShaders(const boost::filesystem::path& solutionDirectory);