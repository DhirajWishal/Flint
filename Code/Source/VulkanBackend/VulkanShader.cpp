// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanShader.hpp"
#include "VulkanBackend/VulkanUtilities.hpp"

#include "spirv_reflect.h"
#include <fstream>

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{/*
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
			}*/

			std::vector<UI32> ResolvePadding(const std::vector<UI32>& code)
			{
				const UI64 finalCodeSize = code.size() / 4;
				std::vector<UI32> resolvedCode(finalCodeSize);
				std::copy(code.begin(), code.begin() + finalCodeSize, resolvedCode.data());

				return resolvedCode;
			}

			void ValidateReflection(SpvReflectResult result)
			{
				switch (result)
				{
				case SPV_REFLECT_RESULT_SUCCESS:
					break;
				case SPV_REFLECT_RESULT_NOT_READY:
					break;
				case SPV_REFLECT_RESULT_ERROR_PARSE_FAILED:
					break;
				case SPV_REFLECT_RESULT_ERROR_ALLOC_FAILED:
					break;
				case SPV_REFLECT_RESULT_ERROR_RANGE_EXCEEDED:
					break;
				case SPV_REFLECT_RESULT_ERROR_NULL_POINTER:
					break;
				case SPV_REFLECT_RESULT_ERROR_INTERNAL_ERROR:
					break;
				case SPV_REFLECT_RESULT_ERROR_COUNT_MISMATCH:
					break;
				case SPV_REFLECT_RESULT_ERROR_ELEMENT_NOT_FOUND:
					break;
				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_CODE_SIZE:
					break;
				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_MAGIC_NUMBER:
					break;
				case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_EOF:
					break;
				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ID_REFERENCE:
					break;
				case SPV_REFLECT_RESULT_ERROR_SPIRV_SET_NUMBER_OVERFLOW:
					break;
				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_STORAGE_CLASS:
					break;
				case SPV_REFLECT_RESULT_ERROR_SPIRV_RECURSION:
					break;
				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_INSTRUCTION:
					break;
				case SPV_REFLECT_RESULT_ERROR_SPIRV_UNEXPECTED_BLOCK_DATA:
					break;
				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_BLOCK_MEMBER_REFERENCE:
					break;
				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_ENTRY_POINT:
					break;
				case SPV_REFLECT_RESULT_ERROR_SPIRV_INVALID_EXECUTION_MODE:
					break;
				default:
					break;
				}
			}
		}

		VulkanShader::VulkanShader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::filesystem::path& path, ShaderCodeType codeType)
			: Shader(pDevice, type, path, codeType)
		{
			FLINT_SETUP_PROFILER();

			ResolveShaderStage();
			std::ifstream shaderFile(path, std::ios::ate | std::ios::binary);

			if (!shaderFile.is_open())
				FLINT_THROW_RUNTIME_ERROR("Submitted shader file path is invalid!");

			UI64 codeSize = shaderFile.tellg();
			shaderFile.seekg(0);

			if (codeType == ShaderCodeType::SPIR_V)
			{
				mShaderCode.resize(codeSize);
				shaderFile.read(reinterpret_cast<char*>(mShaderCode.data()), codeSize);
			}
			else if (codeType == ShaderCodeType::GLSL)
			{
				std::string codeString;
				codeString.resize(codeSize);
				shaderFile.read(codeString.data(), codeSize);
			}

			shaderFile.close();
			CreateShaderModule();
			PerformReflection();
		}

		VulkanShader::VulkanShader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::vector<UI32>& code, ShaderCodeType codeType)
			: Shader(pDevice, type, code, codeType)
		{
			FLINT_SETUP_PROFILER();

			ResolveShaderStage();

			if (codeType != ShaderCodeType::SPIR_V) // TODO
				FLINT_THROW_RUNTIME_ERROR("Invalid shader code type!");

			mShaderCode = code;
			CreateShaderModule();
			PerformReflection();
		}

		VulkanShader::VulkanShader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::string& code, ShaderCodeType codeType)
			: Shader(pDevice, type, code, codeType)
		{
			FLINT_SETUP_PROFILER();

			ResolveShaderStage();
			if (codeType != ShaderCodeType::SPIR_V) // TODO
				FLINT_THROW_RUNTIME_ERROR("Invalid shader code type!");

			mShaderCode.resize(code.size());
			std::copy(code.begin(), code.end(), reinterpret_cast<char*>(mShaderCode.data()));

			CreateShaderModule();
			PerformReflection();
		}

		void VulkanShader::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vkDestroyShaderModule(vDevice.GetLogicalDevice(), vModule, nullptr);
		}

		VkPipelineShaderStageCreateInfo VulkanShader::GetShaderStageCreateInfo() const
		{
			VkPipelineShaderStageCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.pSpecializationInfo = VK_NULL_HANDLE;
			vCreateInfo.pName = "main";
			vCreateInfo.stage = static_cast<VkShaderStageFlagBits>(vStageFlags);
			vCreateInfo.module = vModule;

			return vCreateInfo;
		}

		void VulkanShader::PerformReflection()
		{
			FLINT_SETUP_PROFILER();

			SpvReflectShaderModule sShaderModule = {};
			mShaderCode = _Helpers::ResolvePadding(mShaderCode);
			SpvReflectResult result = spvReflectCreateShaderModule(mShaderCode.size() * sizeof(UI32), mShaderCode.data(), &sShaderModule);

			if (result != SpvReflectResult::SPV_REFLECT_RESULT_SUCCESS)
				return;

			UI32 variableCount = 0;

			uint32_t var_count = 0;
			result = spvReflectEnumerateInputVariables(&sShaderModule, &var_count, NULL);

			SpvReflectInterfaceVariable** input_vars =
				(SpvReflectInterfaceVariable**)malloc(var_count * sizeof(SpvReflectInterfaceVariable*));
			result = spvReflectEnumerateInputVariables(&sShaderModule, &var_count, input_vars);

			free(input_vars);

			spvReflectEnumerateDescriptorBindings(&sShaderModule, &var_count, nullptr);
			std::vector<SpvReflectDescriptorBinding*> pBindings(var_count);
			spvReflectEnumerateDescriptorBindings(&sShaderModule, &var_count, pBindings.data());

			VkDescriptorSetLayoutBinding vBinding = {};
			vBinding.stageFlags = vStageFlags;
			vBinding.pImmutableSamplers = VK_NULL_HANDLE;
			vBinding.descriptorCount = 1;	// TODO

			VkDescriptorPoolSize vSize = {};
			vSize.descriptorCount = 1;

			// Resolve uniform buffers.
			vBinding.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			vSize.type = vBinding.descriptorType;
			for (auto& resource : pBindings)
			{
				vBinding.binding = resource->binding;
				INSERT_INTO_VECTOR(mBindings, vBinding);
				INSERT_INTO_VECTOR(mSizes, vSize);

				const std::string name = resource->name;
				mResources[name] = ShaderResource(vBinding.binding, resource->set, ShaderResourceType::UNIFORM_BUFFER);
			}

			/*
			spirv_cross::Compiler compiler(_Helpers::ResolvePadding(mShaderCode));
			spirv_cross::ShaderResources resources = compiler.get_shader_resources();
			spirv_cross::SPIRType type = {};
			UI64 shaderOffset = 0;

			VkDescriptorSetLayoutBinding vBinding = {};
			vBinding.stageFlags = vStageFlags;
			vBinding.pImmutableSamplers = VK_NULL_HANDLE;
			vBinding.descriptorCount = 1;	// TODO

			VkDescriptorPoolSize vSize = {};
			vSize.descriptorCount = 1;

			// Resolve uniform buffers.
			vBinding.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			vSize.type = vBinding.descriptorType;
			for (auto& resource : resources.uniform_buffers)
			{
				vBinding.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				INSERT_INTO_VECTOR(mBindings, vBinding);
				INSERT_INTO_VECTOR(mSizes, vSize);

				const std::string name = compiler.get_name(resource.id);
				mResources[name] = ShaderResource(vBinding.binding, compiler.get_decoration(resource.id, spv::DecorationDescriptorSet), ShaderResourceType::UNIFORM_BUFFER);
			}

			// Resolve storage buffers.
			vBinding.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			vSize.type = vBinding.descriptorType;
			for (auto& resource : resources.storage_buffers)
			{
				vBinding.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				INSERT_INTO_VECTOR(mBindings, vBinding);
				INSERT_INTO_VECTOR(mSizes, vSize);

				const std::string name = compiler.get_name(resource.id);
				mResources[name] = ShaderResource(vBinding.binding, compiler.get_decoration(resource.id, spv::DecorationDescriptorSet), ShaderResourceType::STORAGE_BUFFER);
			}

			// Resolve samplers.
			vBinding.descriptorType = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			vSize.type = vBinding.descriptorType;
			for (auto& resource : resources.sampled_images)
			{
				vBinding.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				INSERT_INTO_VECTOR(mBindings, vBinding);
				INSERT_INTO_VECTOR(mSizes, vSize);

				const std::string name = compiler.get_name(resource.id);
				mResources[name] = ShaderResource(vBinding.binding, compiler.get_decoration(resource.id, spv::DecorationDescriptorSet), ShaderResourceType::SAMPLER);
			}

			// Resolve shader inputs.
			for (auto& resource : resources.stage_inputs)
			{
				auto& Ty = compiler.get_type(resource.base_type_id);
				INSERT_INTO_VECTOR(
					mInputAttributes[compiler.get_decoration(resource.id, spv::DecorationBinding)],
					ShaderAttribute(
						compiler.get_name(resource.id),
						compiler.get_decoration(resource.id, spv::DecorationLocation),
						static_cast<ShaderAttributeDataType>((static_cast<UI64>(Ty.width) / 8) * Ty.vecsize)));
			}

			// Resolve shader outputs.
			for (auto& resource : resources.stage_outputs)
			{
				auto& Ty = compiler.get_type(resource.base_type_id);
				INSERT_INTO_VECTOR(
					mOutputAttributes[compiler.get_decoration(resource.id, spv::DecorationBinding)],
					ShaderAttribute(
						compiler.get_name(resource.id),
						compiler.get_decoration(resource.id, spv::DecorationLocation),
						static_cast<ShaderAttributeDataType>((static_cast<UI64>(Ty.width) / 8) * Ty.vecsize)));
			}

			VkPushConstantRange vPushConstantRange = {};
			vPushConstantRange.stageFlags = vStageFlags;
			vPushConstantRange.offset = 0;
			for (auto& resource : resources.push_constant_buffers)
			{
				auto name = compiler.get_name(resource.id);
				auto& Ty = compiler.get_type(resource.base_type_id);
				vPushConstantRange.size = (static_cast<UI64>(Ty.width) / 8) * Ty.vecsize;
				INSERT_INTO_VECTOR(mConstantRanges, vPushConstantRange);

				vPushConstantRange.offset += vPushConstantRange.size;
			}*/
		}

		void VulkanShader::ResolveShaderStage()
		{
			vStageFlags = Utilities::GetShaderStage(mType);
		}

		void VulkanShader::CreateShaderModule()
		{
			VkShaderModuleCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.codeSize = mShaderCode.size();
			vCreateInfo.pCode = mShaderCode.data();

			FLINT_VK_ASSERT(vkCreateShaderModule(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), &vCreateInfo, nullptr, &vModule));
		}
	}
}