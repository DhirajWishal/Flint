// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Instance.hpp"
#include "Flint/VertexBuffer.hpp"

#include "VulkanBackend/VulkanInstance.hpp"

namespace Flint
{
    Instance &CreateInstance(bool enableValidation)
    {
        return *new VulkanBackend::VulkanInstance(enableValidation);
    }

    void DestroyInstance(Instance &instance)
    {
        instance.Terminate();
        delete &instance;
    }

    UI64 VertexDescriptor::Stride() const
    {
        UI64 size = 0;
        for (auto attribute : mAttributeTypes)
            size += static_cast<UI8>(attribute);

        return size;
    }
}