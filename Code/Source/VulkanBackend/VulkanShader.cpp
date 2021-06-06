// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanShader.hpp"

#define SHADERC_SHAREDLIB
#include <shaderc/shaderc.hpp>
#include <spirv_cross.hpp>
#include <fstream>

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			shaderc_shader_kind GLSLShaderStageToShadercKind(VkShaderStageFlagBits flag)
			{
				switch (flag)
				{
				case VK_SHADER_STAGE_VERTEX_BIT:
					return shaderc_shader_kind::shaderc_glsl_vertex_shader;

				case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
					return shaderc_shader_kind::shaderc_glsl_tess_control_shader;

				case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
					return shaderc_shader_kind::shaderc_glsl_tess_evaluation_shader;

				case VK_SHADER_STAGE_GEOMETRY_BIT:
					return shaderc_shader_kind::shaderc_glsl_geometry_shader;

				case VK_SHADER_STAGE_FRAGMENT_BIT:
					return shaderc_shader_kind::shaderc_glsl_fragment_shader;

				case VK_SHADER_STAGE_COMPUTE_BIT:
					return shaderc_shader_kind::shaderc_glsl_compute_shader;

				case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
					return shaderc_shader_kind::shaderc_glsl_raygen_shader;

				case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
					return shaderc_shader_kind::shaderc_glsl_anyhit_shader;

				case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
					return shaderc_shader_kind::shaderc_glsl_closesthit_shader;

				case VK_SHADER_STAGE_MISS_BIT_KHR:
					return shaderc_shader_kind::shaderc_glsl_miss_shader;

				case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
					return shaderc_shader_kind::shaderc_glsl_intersection_shader;

				case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
					return shaderc_shader_kind::shaderc_glsl_callable_shader;

				case VK_SHADER_STAGE_TASK_BIT_NV:
					return shaderc_shader_kind::shaderc_glsl_task_shader;

				case VK_SHADER_STAGE_MESH_BIT_NV:
					return shaderc_shader_kind::shaderc_glsl_mesh_shader;

				default:
					FLINT_THROW_RUNTIME_ERROR("Unsupported shader stage!");
					break;
				}

				return shaderc_shader_kind::shaderc_glsl_vertex_shader;
			}

			std::vector<UI32> ResolvePadding(const std::vector<UI32>& code)
			{
				const UI64 finalCodeSize = code.size() / 4;
				std::vector<UI32> resolvedCode(finalCodeSize);
				std::copy(code.begin(), code.begin() + finalCodeSize, resolvedCode.data());

				return resolvedCode;
			}
		}

		VulkanShader::VulkanShader(VulkanDevice& device, VkShaderStageFlags stageFlags, const std::filesystem::path& path, ShaderCodeType type) : vDevice(device), vStageFlags(stageFlags)
		{
			std::ifstream shaderFile(path, std::ios::ate | std::ios::binary);

			if (!shaderFile.is_open())
				FLINT_THROW_RUNTIME_ERROR("Submitted shader file path is invalid!");

			UI64 codeSize = shaderFile.tellg();
			shaderFile.seekg(0);

			if (type == ShaderCodeType::SPIR_V)
			{
				mShaderCode.resize(codeSize);
				shaderFile.read(reinterpret_cast<char*>(mShaderCode.data()), codeSize);
			}
			else if (type == ShaderCodeType::GLSL)
			{
				std::string codeString;
				codeString.resize(codeSize);
				shaderFile.read(codeString.data(), codeSize);

				shaderc::Compiler compiler = {};
				auto result = compiler.CompileGlslToSpv(codeString, _Helpers::GLSLShaderStageToShadercKind(static_cast<VkShaderStageFlagBits>(stageFlags)), "InputFile.txt");

				if (result.GetNumErrors())
					FLINT_THROW_RUNTIME_ERROR("Errors while compiling GLSL to SPIRV!");
			}

			shaderFile.close();
			CreateShaderModule();
		}

		VulkanShader::VulkanShader(VulkanDevice& device, VkShaderStageFlags stageFlags, const std::vector<UI32>& code, ShaderCodeType type) : vDevice(device), vStageFlags(stageFlags)
		{
			if (type != ShaderCodeType::SPIR_V) // TODO
				FLINT_THROW_RUNTIME_ERROR("Invalid shader code type!");

			mShaderCode = code;
			CreateShaderModule();
		}

		VulkanShader::VulkanShader(VulkanDevice& device, VkShaderStageFlags stageFlags, const std::string& code, ShaderCodeType type) : vDevice(device), vStageFlags(stageFlags)
		{
			if (type != ShaderCodeType::SPIR_V) // TODO
				FLINT_THROW_RUNTIME_ERROR("Invalid shader code type!");

			mShaderCode.resize(code.size());
			std::copy(code.begin(), code.end(), reinterpret_cast<char*>(mShaderCode.data()));

			CreateShaderModule();
		}

		void VulkanShader::Terminate()
		{
			vkDestroyShaderModule(vDevice.GetLogicalDevice(), vModule, nullptr);
		}

		VkPipelineShaderStageCreateInfo VulkanShader::GetShaderStageCreateInfo() const
		{
			VkPipelineShaderStageCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = VK_NULL_HANDLE;
			vCreateInfo.pSpecializationInfo = VK_NULL_HANDLE;
			vCreateInfo.pName = "main";
			vCreateInfo.stage = static_cast<VkShaderStageFlagBits>(vStageFlags);
			vCreateInfo.module = vModule;

			return vCreateInfo;
		}

		ShaderDescriptor VulkanShader::PerformReflection() const
		{
			ShaderDescriptor descriptor = {};
			spirv_cross::Compiler compiler(_Helpers::ResolvePadding(mShaderCode));
			spirv_cross::ShaderResources resources = compiler.get_shader_resources();
			spirv_cross::SPIRType type = {};
			UI64 shaderOffset = 0;

			for (auto& resource : resources.uniform_buffers)
				INSERT_INTO_VECTOR(descriptor.mResources, ShaderResource(compiler.get_name(resource.id), compiler.get_decoration(resource.id, spv::DecorationBinding), ShaderResourceType::UNIFORM_BUFFER));

			for (auto& resource : resources.storage_buffers)
				INSERT_INTO_VECTOR(descriptor.mResources, ShaderResource(compiler.get_name(resource.id), compiler.get_decoration(resource.id, spv::DecorationBinding), ShaderResourceType::STORAGE_BUFFER));

			for (auto& resource : resources.sampled_images)
				INSERT_INTO_VECTOR(descriptor.mResources, ShaderResource(compiler.get_name(resource.id), compiler.get_decoration(resource.id, spv::DecorationBinding), ShaderResourceType::SAMPLER));

			for (auto& resource : resources.stage_inputs)
			{
				auto& Ty = compiler.get_type(resource.base_type_id);
				UI64 size = (static_cast<UI64>(Ty.width) / 8) * (Ty.vecsize == 3 ? 4 : Ty.vecsize);
				INSERT_INTO_VECTOR(descriptor.mInputAttributes, ShaderAttribute(compiler.get_name(resource.id), compiler.get_decoration(resource.id, spv::DecorationBinding), static_cast<ShaderAttributeDataType>(size)));
			}

			for (auto& resource : resources.stage_outputs)
			{
				auto& Ty = compiler.get_type(resource.base_type_id);
				UI64 size = (static_cast<UI64>(Ty.width) / 8) * (Ty.vecsize == 3 ? 4 : Ty.vecsize);
				INSERT_INTO_VECTOR(descriptor.mOutputAttributes, ShaderAttribute(compiler.get_name(resource.id), compiler.get_decoration(resource.id, spv::DecorationBinding), static_cast<ShaderAttributeDataType>(size)));
			}

			return descriptor;
		}

		void VulkanShader::CreateShaderModule()
		{
			VkShaderModuleCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = VK_NULL_HANDLE;
			vCreateInfo.codeSize = mShaderCode.size();
			vCreateInfo.pCode = mShaderCode.data();

			FLINT_VK_ASSERT(vkCreateShaderModule(vDevice.GetLogicalDevice(), &vCreateInfo, nullptr, &vModule));
		}
	}
}