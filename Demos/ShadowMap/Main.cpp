// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Components/GameObject.hpp"
#include "Components/ImGui.hpp"
#include "Objects/Preview.hpp"
#include "Objects/Light.hpp"
#include "ImGuizmo.h"

#include <iostream>

int main(int argc, char** argv)
{
	try
	{
		SceneState mState{ "Flint: ShadowMap" };

		{
			Light* pLight = nullptr;
			Preview* pPreview = nullptr;

			ImGUI mImGui(glm::vec3(0.0f), &mState);
			boost::container::vector<std::unique_ptr<GameObject>> pGameObjects;
			pGameObjects.push_back(std::make_unique<Light>(glm::vec3(1.0f), &mState));
			pGameObjects.push_back(std::make_unique<Preview>(glm::vec3(0.0f), &mState, boost::filesystem::path("E:\\Dynamik\\Game Repository\\InHouse\\DemoScene.obj"), boost::container::vector<boost::filesystem::path>{}));

			pLight = static_cast<Light*>(pGameObjects[0].get());
			pPreview = static_cast<Preview*>(pGameObjects[1].get());

			std::chrono::time_point<std::chrono::high_resolution_clock> oldTimePoint = std::chrono::high_resolution_clock::now();

			mState.PrepareRenderTargetsToDraw();
			while (mState.pDisplay->IsOpen())
			{
				FLINT_SETUP_PROFILER();
				std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
				UI64 delta = now.time_since_epoch().count() - oldTimePoint.time_since_epoch().count();

				mState.PrepareNewFrame();

				ImGui::NewFrame();
				ImGuizmo::SetOrthographic(false);
				ImGuizmo::BeginFrame();

				float frameTime = delta / (1000.0f * 1000.0f);
				ImGui::Text("Frame Time: %f ms", frameTime);

				ImGuiIO& io = ImGui::GetIO();

				{
					io.DeltaTime = frameTime / 1000.0f;

					auto extent = mState.pDisplay->GetExtent();
					if (!extent.IsZero())
						io.DisplaySize = ImVec2(static_cast<float>(extent.mWidth), static_cast<float>(extent.mHeight));

					auto position = mState.pDisplay->GetMousePosition();
					io.MousePos = ImVec2(position.X, position.Y);

					if (mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::LEFT).IsPressed() || mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::LEFT).IsOnRepeat())
						io.MouseDown[0] = true;
					else if (mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::LEFT).IsReleased())
						io.MouseDown[0] = false;

					if (mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::RIGHT).IsPressed() || mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::RIGHT).IsOnRepeat())
						io.MouseDown[1] = true;
					else if (mState.pDisplay->GetMouseButtonEvent(Flint::MouseButton::RIGHT).IsReleased())
						io.MouseDown[1] = false;
				}

				if (!io.WantCaptureMouse)
					mState.UpdateCamera(delta);

				//for (auto& pGameObject : pGameObjects)
				//	pGameObject->OnUpdate(delta);

				pLight->OnUpdate(delta);

				pPreview->OnUpdate(delta);
				pPreview->SetLightPosition(pLight->GetPosition());
				//pLight->SetPosition(pPreview->GetLightPosition());

				ImGui::Render();
				mImGui.OnUpdate(0);

				oldTimePoint = now;
				mState.SubmitFrames();
				mState.pDisplay->Update();
			}

			int finish = true;
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}

/*
WARNING: [Loader Message] Code 0 : loaderAddLayerProperties: E:\SDKs\Vulkan\1.2.182.0\Bin\VkLayer_api_dump.json invalid layer manifest file version 1.2.0.  May cause errors.
WARNING: [Loader Message] Code 0 : loaderAddLayerProperties: E:\SDKs\Vulkan\1.2.182.0\Bin\VkLayer_device_simulation.json invalid layer manifest file version 1.2.0.  May cause errors.
WARNING: [Loader Message] Code 0 : loaderAddLayerProperties: E:\SDKs\Vulkan\1.2.182.0\Bin\VkLayer_gfxreconstruct.json invalid layer manifest file version 1.2.0.  May cause errors.
WARNING: [Loader Message] Code 0 : loaderAddLayerProperties: E:\SDKs\Vulkan\1.2.182.0\Bin\VkLayer_khronos_synchronization2.json invalid layer manifest file version 1.2.0.  May cause errors.
WARNING: [Loader Message] Code 0 : loaderAddLayerProperties: E:\SDKs\Vulkan\1.2.182.0\Bin\VkLayer_khronos_validation.json invalid layer manifest file version 1.2.0.  May cause errors.
WARNING: [Loader Message] Code 0 : loaderAddLayerProperties: E:\SDKs\Vulkan\1.2.182.0\Bin\VkLayer_screenshot.json invalid layer manifest file version 1.2.0.  May cause errors.
==========
VULKANINFO
==========

Vulkan Instance Version: 1.2.170


Instance Extensions: count = 15
===============================
        VK_EXT_debug_report                    : extension revision 9
        VK_EXT_debug_utils                     : extension revision 2
        VK_EXT_swapchain_colorspace            : extension revision 4
        VK_KHR_device_group_creation           : extension revision 1
        VK_KHR_display                         : extension revision 23
        VK_KHR_external_fence_capabilities     : extension revision 1
        VK_KHR_external_memory_capabilities    : extension revision 1
        VK_KHR_external_semaphore_capabilities : extension revision 1
        VK_KHR_get_display_properties2         : extension revision 1
        VK_KHR_get_physical_device_properties2 : extension revision 2
        VK_KHR_get_surface_capabilities2       : extension revision 1
        VK_KHR_surface                         : extension revision 25
        VK_KHR_surface_protected_capabilities  : extension revision 1
        VK_KHR_win32_surface                   : extension revision 6
        VK_NV_external_memory_capabilities     : extension revision 1

Layers: count = 14
==================
VK_LAYER_EOS_Overlay (Vulkan overlay layer for Epic Online Services) Vulkan version 1.2.136, layer version 1:
        Layer Extensions: count = 0
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 0

VK_LAYER_EOS_Overlay (Vulkan overlay layer for Epic Online Services) Vulkan version 1.2.136, layer version 1:
        Layer Extensions: count = 0
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 0

VK_LAYER_KHRONOS_synchronization2 (Khronos Synchronization2 layer) Vulkan version 1.2.182, layer version 1:
        Layer Extensions: count = 0
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 1
                        VK_KHR_synchronization2 : extension revision 1

VK_LAYER_KHRONOS_validation (Khronos Validation Layer) Vulkan version 1.2.182, layer version 1:
        Layer Extensions: count = 3
                VK_EXT_debug_report        : extension revision 9
                VK_EXT_debug_utils         : extension revision 1
                VK_EXT_validation_features : extension revision 2
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 3
                        VK_EXT_debug_marker     : extension revision 4
                        VK_EXT_tooling_info     : extension revision 1
                        VK_EXT_validation_cache : extension revision 1

VK_LAYER_LUNARG_api_dump (LunarG API dump layer) Vulkan version 1.2.182, layer version 2:
        Layer Extensions: count = 0
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 1
                        VK_EXT_tooling_info : extension revision 1

VK_LAYER_LUNARG_device_simulation (LunarG device simulation layer) Vulkan version 1.2.182, layer version 1:
        Layer Extensions: count = 0
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 1
                        VK_EXT_tooling_info : extension revision 1

VK_LAYER_LUNARG_gfxreconstruct (GFXReconstruct Capture Layer Version 0.9.8) Vulkan version 1.2.182, layer version 36872:
        Layer Extensions: count = 0
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 1
                        VK_EXT_tooling_info : extension revision 1

VK_LAYER_LUNARG_monitor (Execution Monitoring Layer) Vulkan version 1.2.182, layer version 1:
        Layer Extensions: count = 0
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 1
                        VK_EXT_tooling_info : extension revision 1

VK_LAYER_LUNARG_screenshot (LunarG image capture layer) Vulkan version 1.2.182, layer version 1:
        Layer Extensions: count = 0
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 1
                        VK_EXT_tooling_info : extension revision 1

VK_LAYER_NV_GPU_Trace_release_public_2020_3_1 (NVIDIA Nsight Graphics GPU Trace interception layer) Vulkan version 1.2.131, layer version 1:
        Layer Extensions: count = 1
                VK_EXT_debug_utils : extension revision 1
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 1
                        VK_EXT_debug_marker : extension revision 3

VK_LAYER_NV_nomad_release_public_2020_3_1 (NVIDIA Nsight Graphics interception layer) Vulkan version 1.2.131, layer version 1:
        Layer Extensions: count = 1
                VK_EXT_debug_utils : extension revision 1
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 2
                        VK_EXT_debug_marker : extension revision 3
                        VK_EXT_tooling_info : extension revision 1

VK_LAYER_NV_nsight-sys (NVIDIA Nsight Systems profiler layer) Vulkan version 1.1.97, layer version 2:
        Layer Extensions: count = 2
                VK_EXT_debug_report : extension revision 9
                VK_EXT_debug_utils  : extension revision 1
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 3
                        VK_EXT_debug_marker : extension revision 4
                        VK_KHR_swapchain    : extension revision 70
                        VK_NV_ray_tracing   : extension revision 3

VK_LAYER_NV_optimus (NVIDIA Optimus layer) Vulkan version 1.2.175, layer version 1:
        Layer Extensions: count = 0
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 0

VK_LAYER_RENDERDOC_Capture (Debugging capture layer for RenderDoc) Vulkan version 1.2.131, layer version 15:
        Layer Extensions: count = 1
                VK_EXT_debug_utils : extension revision 1
        Devices: count = 1
                GPU id = 0 (NVIDIA GeForce GTX 1660 SUPER)
                Layer-Device Extensions: count = 2
                        VK_EXT_debug_marker : extension revision 4
                        VK_EXT_tooling_info : extension revision 1

Presentable Surfaces:
=====================
GPU id : 0 (NVIDIA GeForce GTX 1660 SUPER):
        Surface type = VK_KHR_win32_surface
        Formats: count = 3
                SurfaceFormat[0]:
                        format = FORMAT_B8G8R8A8_UNORM
                        colorSpace = COLOR_SPACE_SRGB_NONLINEAR_KHR
                SurfaceFormat[1]:
                        format = FORMAT_B8G8R8A8_SRGB
                        colorSpace = COLOR_SPACE_SRGB_NONLINEAR_KHR
                SurfaceFormat[2]:
                        format = FORMAT_A2B10G10R10_UNORM_PACK32
                        colorSpace = COLOR_SPACE_SRGB_NONLINEAR_KHR
        Present Modes: count = 4
                PRESENT_MODE_FIFO_KHR
                PRESENT_MODE_FIFO_RELAXED_KHR
                PRESENT_MODE_MAILBOX_KHR
                PRESENT_MODE_IMMEDIATE_KHR
        VkSurfaceCapabilitiesKHR:
        -------------------------
                minImageCount       = 2
                maxImageCount       = 8
                currentExtent:
                        width  = 256
                        height = 256
                minImageExtent:
                        width  = 256
                        height = 256
                maxImageExtent:
                        width  = 256
                        height = 256
                maxImageArrayLayers = 1
                supportedTransforms: count = 1
                        SURFACE_TRANSFORM_IDENTITY_BIT_KHR
                currentTransform    = SURFACE_TRANSFORM_IDENTITY_BIT_KHR
                supportedCompositeAlpha: count = 1
                        COMPOSITE_ALPHA_OPAQUE_BIT_KHR
                supportedUsageFlags: count = 6
                        IMAGE_USAGE_TRANSFER_SRC_BIT
                        IMAGE_USAGE_TRANSFER_DST_BIT
                        IMAGE_USAGE_SAMPLED_BIT
                        IMAGE_USAGE_STORAGE_BIT
                        IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                        IMAGE_USAGE_INPUT_ATTACHMENT_BIT
        VkSurfaceCapabilitiesFullScreenExclusiveEXT:
        --------------------------------------------
                fullScreenExclusiveSupported = true

        VkSurfaceProtectedCapabilitiesKHR:
        ----------------------------------
                supportsProtected = false



Device Groups:
==============
Group 0:
        Properties:
                physicalDevices: count = 1
                        NVIDIA GeForce GTX 1660 SUPER (ID: 0)
                subsetAllocation = 1

        Present Capabilities:
                NVIDIA GeForce GTX 1660 SUPER (ID: 0):
                        Can present images from the following devices: count = 1
                                NVIDIA GeForce GTX 1660 SUPER (ID: 0)
                Present modes: count = 1
                        DEVICE_GROUP_PRESENT_MODE_LOCAL_BIT_KHR


Device Properties and Extensions:
=================================
GPU0:
VkPhysicalDeviceProperties:
---------------------------
        apiVersion     = 4202671 (1.2.175)
        driverVersion  = 1976631296 (0x75d10000)
        vendorID       = 0x10de
        deviceID       = 0x21c4
        deviceType     = PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
        deviceName     = NVIDIA GeForce GTX 1660 SUPER

VkPhysicalDeviceLimits:
-----------------------
        maxImageDimension1D                             = 32768
        maxImageDimension2D                             = 32768
        maxImageDimension3D                             = 16384
        maxImageDimensionCube                           = 32768
        maxImageArrayLayers                             = 2048
        maxTexelBufferElements                          = 134217728
        maxUniformBufferRange                           = 65536
        maxStorageBufferRange                           = 4294967295
        maxPushConstantsSize                            = 256
        maxMemoryAllocationCount                        = 4096
        maxSamplerAllocationCount                       = 4000
        bufferImageGranularity                          = 0x00000400
        sparseAddressSpaceSize                          = 0xffffffffff
        maxBoundDescriptorSets                          = 32
        maxPerStageDescriptorSamplers                   = 1048576
        maxPerStageDescriptorUniformBuffers             = 1048576
        maxPerStageDescriptorStorageBuffers             = 1048576
        maxPerStageDescriptorSampledImages              = 1048576
        maxPerStageDescriptorStorageImages              = 1048576
        maxPerStageDescriptorInputAttachments           = 1048576
        maxPerStageResources                            = 4294967295
        maxDescriptorSetSamplers                        = 1048576
        maxDescriptorSetUniformBuffers                  = 1048576
        maxDescriptorSetUniformBuffersDynamic           = 15
        maxDescriptorSetStorageBuffers                  = 1048576
        maxDescriptorSetStorageBuffersDynamic           = 16
        maxDescriptorSetSampledImages                   = 1048576
        maxDescriptorSetStorageImages                   = 1048576
        maxDescriptorSetInputAttachments                = 1048576
        maxVertexInputAttributes                        = 32
        maxVertexInputBindings                          = 32
        maxVertexInputAttributeOffset                   = 2047
        maxVertexInputBindingStride                     = 2048
        maxVertexOutputComponents                       = 128
        maxTessellationGenerationLevel                  = 64
        maxTessellationPatchSize                        = 32
        maxTessellationControlPerVertexInputComponents  = 128
        maxTessellationControlPerVertexOutputComponents = 128
        maxTessellationControlPerPatchOutputComponents  = 120
        maxTessellationControlTotalOutputComponents     = 4216
        maxTessellationEvaluationInputComponents        = 128
        maxTessellationEvaluationOutputComponents       = 128
        maxGeometryShaderInvocations                    = 32
        maxGeometryInputComponents                      = 128
        maxGeometryOutputComponents                     = 128
        maxGeometryOutputVertices                       = 1024
        maxGeometryTotalOutputComponents                = 1024
        maxFragmentInputComponents                      = 128
        maxFragmentOutputAttachments                    = 8
        maxFragmentDualSrcAttachments                   = 1
        maxFragmentCombinedOutputResources              = 16
        maxComputeSharedMemorySize                      = 49152
        maxComputeWorkGroupCount: count = 3
                2147483647
                65535
                65535
        maxComputeWorkGroupInvocations                  = 1024
        maxComputeWorkGroupSize: count = 3
                1024
                1024
                64
        subPixelPrecisionBits                           = 8
        subTexelPrecisionBits                           = 8
        mipmapPrecisionBits                             = 8
        maxDrawIndexedIndexValue                        = 4294967295
        maxDrawIndirectCount                            = 4294967295
        maxSamplerLodBias                               = 15
        maxSamplerAnisotropy                            = 16
        maxViewports                                    = 16
        maxViewportDimensions: count = 2
                32768
                32768
        viewportBoundsRange: count = 2
                -65536
                65536
        viewportSubPixelBits                            = 8
        minMemoryMapAlignment                           = 64
        minTexelBufferOffsetAlignment                   = 0x00000010
        minUniformBufferOffsetAlignment                 = 0x00000040
        minStorageBufferOffsetAlignment                 = 0x00000010
        minTexelOffset                                  = -8
        maxTexelOffset                                  = 7
        minTexelGatherOffset                            = -32
        maxTexelGatherOffset                            = 31
        minInterpolationOffset                          = -0.5
        maxInterpolationOffset                          = 0.4375
        subPixelInterpolationOffsetBits                 = 4
        maxFramebufferWidth                             = 32768
        maxFramebufferHeight                            = 32768
        maxFramebufferLayers                            = 2048
        framebufferColorSampleCounts: count = 4
                SAMPLE_COUNT_1_BIT
                SAMPLE_COUNT_2_BIT
                SAMPLE_COUNT_4_BIT
                SAMPLE_COUNT_8_BIT
        framebufferDepthSampleCounts: count = 4
                SAMPLE_COUNT_1_BIT
                SAMPLE_COUNT_2_BIT
                SAMPLE_COUNT_4_BIT
                SAMPLE_COUNT_8_BIT
        framebufferStencilSampleCounts: count = 5
                SAMPLE_COUNT_1_BIT
                SAMPLE_COUNT_2_BIT
                SAMPLE_COUNT_4_BIT
                SAMPLE_COUNT_8_BIT
                SAMPLE_COUNT_16_BIT
        framebufferNoAttachmentsSampleCounts: count = 5
                SAMPLE_COUNT_1_BIT
                SAMPLE_COUNT_2_BIT
                SAMPLE_COUNT_4_BIT
                SAMPLE_COUNT_8_BIT
                SAMPLE_COUNT_16_BIT
        maxColorAttachments                             = 8
        sampledImageColorSampleCounts: count = 4
                SAMPLE_COUNT_1_BIT
                SAMPLE_COUNT_2_BIT
                SAMPLE_COUNT_4_BIT
                SAMPLE_COUNT_8_BIT
        sampledImageIntegerSampleCounts: count = 4
                SAMPLE_COUNT_1_BIT
                SAMPLE_COUNT_2_BIT
                SAMPLE_COUNT_4_BIT
                SAMPLE_COUNT_8_BIT
        sampledImageDepthSampleCounts: count = 4
                SAMPLE_COUNT_1_BIT
                SAMPLE_COUNT_2_BIT
                SAMPLE_COUNT_4_BIT
                SAMPLE_COUNT_8_BIT
        sampledImageStencilSampleCounts: count = 5
                SAMPLE_COUNT_1_BIT
                SAMPLE_COUNT_2_BIT
                SAMPLE_COUNT_4_BIT
                SAMPLE_COUNT_8_BIT
                SAMPLE_COUNT_16_BIT
        storageImageSampleCounts: count = 4
                SAMPLE_COUNT_1_BIT
                SAMPLE_COUNT_2_BIT
                SAMPLE_COUNT_4_BIT
                SAMPLE_COUNT_8_BIT
        maxSampleMaskWords                              = 1
        timestampComputeAndGraphics                     = true
        timestampPeriod                                 = 1
        maxClipDistances                                = 8
        maxCullDistances                                = 8
        maxCombinedClipAndCullDistances                 = 8
        discreteQueuePriorities                         = 2
        pointSizeRange: count = 2
                1
                2047.94
        lineWidthRange: count = 2
                1
                64
        pointSizeGranularity                            = 0.0625
        lineWidthGranularity                            = 0.0625
        strictLines                                     = true
        standardSampleLocations                         = true
        optimalBufferCopyOffsetAlignment                = 0x00000001
        optimalBufferCopyRowPitchAlignment              = 0x00000001
        nonCoherentAtomSize                             = 0x00000040

VkPhysicalDeviceSparseProperties:
---------------------------------
        residencyStandard2DBlockShape            = true
        residencyStandard2DMultisampleBlockShape = true
        residencyStandard3DBlockShape            = true
        residencyAlignedMipSize                  = false
        residencyNonResidentStrict               = true

VkPhysicalDeviceAccelerationStructurePropertiesKHR:
---------------------------------------------------
        maxGeometryCount                                           = 16777215
        maxInstanceCount                                           = 16777215
        maxPrimitiveCount                                          = 536870911
        maxPerStageDescriptorAccelerationStructures                = 1048576
        maxPerStageDescriptorUpdateAfterBindAccelerationStructures = 1048576
        maxDescriptorSetAccelerationStructures                     = 1048576
        maxDescriptorSetUpdateAfterBindAccelerationStructures      = 1048576
        minAccelerationStructureScratchOffsetAlignment             = 128

VkPhysicalDeviceBlendOperationAdvancedPropertiesEXT:
----------------------------------------------------
        advancedBlendMaxColorAttachments      = 8
        advancedBlendIndependentBlend         = false
        advancedBlendNonPremultipliedSrcColor = true
        advancedBlendNonPremultipliedDstColor = true
        advancedBlendCorrelatedOverlap        = true
        advancedBlendAllOperations            = true

VkPhysicalDeviceConservativeRasterizationPropertiesEXT:
-------------------------------------------------------
        primitiveOverestimationSize                 = 0.00195313
        maxExtraPrimitiveOverestimationSize         = 0.75
        extraPrimitiveOverestimationSizeGranularity = 0.25
        primitiveUnderestimation                    = true
        conservativePointAndLineRasterization       = true
        degenerateTrianglesRasterized               = true
        degenerateLinesRasterized                   = true
        fullyCoveredFragmentShaderInputVariable     = true
        conservativeRasterizationPostDepthCoverage  = true

VkPhysicalDeviceCustomBorderColorPropertiesEXT:
-----------------------------------------------
        maxCustomBorderColorSamplers = 4000

VkPhysicalDeviceDepthStencilResolveProperties:
----------------------------------------------
        supportedDepthResolveModes: count = 4
                RESOLVE_MODE_SAMPLE_ZERO_BIT
                RESOLVE_MODE_AVERAGE_BIT
                RESOLVE_MODE_MIN_BIT
                RESOLVE_MODE_MAX_BIT
        supportedStencilResolveModes: count = 3
                RESOLVE_MODE_SAMPLE_ZERO_BIT
                RESOLVE_MODE_MIN_BIT
                RESOLVE_MODE_MAX_BIT
        independentResolveNone = true
        independentResolve     = true

VkPhysicalDeviceDescriptorIndexingProperties:
---------------------------------------------
        maxUpdateAfterBindDescriptorsInAllPools              = 4294967295
        shaderUniformBufferArrayNonUniformIndexingNative     = true
        shaderSampledImageArrayNonUniformIndexingNative      = true
        shaderStorageBufferArrayNonUniformIndexingNative     = true
        shaderStorageImageArrayNonUniformIndexingNative      = true
        shaderInputAttachmentArrayNonUniformIndexingNative   = true
        robustBufferAccessUpdateAfterBind                    = true
        quadDivergentImplicitLod                             = true
        maxPerStageDescriptorUpdateAfterBindSamplers         = 1048576
        maxPerStageDescriptorUpdateAfterBindUniformBuffers   = 1048576
        maxPerStageDescriptorUpdateAfterBindStorageBuffers   = 1048576
        maxPerStageDescriptorUpdateAfterBindSampledImages    = 1048576
        maxPerStageDescriptorUpdateAfterBindStorageImages    = 1048576
        maxPerStageDescriptorUpdateAfterBindInputAttachments = 1048576
        maxPerStageUpdateAfterBindResources                  = 4294967295
        maxDescriptorSetUpdateAfterBindSamplers              = 1048576
        maxDescriptorSetUpdateAfterBindUniformBuffers        = 1048576
        maxDescriptorSetUpdateAfterBindUniformBuffersDynamic = 15
        maxDescriptorSetUpdateAfterBindStorageBuffers        = 1048576
        maxDescriptorSetUpdateAfterBindStorageBuffersDynamic = 16
        maxDescriptorSetUpdateAfterBindSampledImages         = 1048576
        maxDescriptorSetUpdateAfterBindStorageImages         = 1048576
        maxDescriptorSetUpdateAfterBindInputAttachments      = 1048576

VkPhysicalDeviceDiscardRectanglePropertiesEXT:
----------------------------------------------
        maxDiscardRectangles = 8

VkPhysicalDeviceDriverProperties:
---------------------------------
        driverID           = DRIVER_ID_NVIDIA_PROPRIETARY
        driverName         = NVIDIA
        driverInfo         = 471.68
        conformanceVersion = 1.2.6.0

VkPhysicalDeviceExternalMemoryHostPropertiesEXT:
------------------------------------------------
        minImportedHostPointerAlignment = 0x00001000

VkPhysicalDeviceFloatControlsProperties:
----------------------------------------
        denormBehaviorIndependence            = SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL
        roundingModeIndependence              = SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL
        shaderSignedZeroInfNanPreserveFloat16 = true
        shaderSignedZeroInfNanPreserveFloat32 = true
        shaderSignedZeroInfNanPreserveFloat64 = true
        shaderDenormPreserveFloat16           = true
        shaderDenormPreserveFloat32           = false
        shaderDenormPreserveFloat64           = false
        shaderDenormFlushToZeroFloat16        = false
        shaderDenormFlushToZeroFloat32        = false
        shaderDenormFlushToZeroFloat64        = false
        shaderRoundingModeRTEFloat16          = true
        shaderRoundingModeRTEFloat32          = true
        shaderRoundingModeRTEFloat64          = true
        shaderRoundingModeRTZFloat16          = false
        shaderRoundingModeRTZFloat32          = true
        shaderRoundingModeRTZFloat64          = true

VkPhysicalDeviceFragmentShadingRatePropertiesKHR:
-------------------------------------------------
        minFragmentShadingRateAttachmentTexelSize:
                width  = 16
                height = 16
        maxFragmentShadingRateAttachmentTexelSize:
                width  = 16
                height = 16
        maxFragmentShadingRateAttachmentTexelSizeAspectRatio = 1
        primitiveFragmentShadingRateWithMultipleViewports    = false
        layeredShadingRateAttachments                        = true
        fragmentShadingRateNonTrivialCombinerOps             = true
        maxFragmentSize:
                width  = 4
                height = 4
        maxFragmentSizeAspectRatio                           = 2
        maxFragmentShadingRateCoverageSamples                = 16
        maxFragmentShadingRateRasterizationSamples           = SAMPLE_COUNT_16_BIT
        fragmentShadingRateWithShaderDepthStencilWrites      = true
        fragmentShadingRateWithSampleMask                    = true
        fragmentShadingRateWithShaderSampleMask              = true
        fragmentShadingRateWithConservativeRasterization     = true
        fragmentShadingRateWithFragmentShaderInterlock       = true
        fragmentShadingRateWithCustomSampleLocations         = true
        fragmentShadingRateStrictMultiplyCombiner            = true

VkPhysicalDeviceIDProperties:
-----------------------------
        deviceUUID      = 09677296-49dc-ce27-f332-7de5c453bfea
        driverUUID      = d1a0dc91-7a44-affa-d760-3617b3a9d3d9
        deviceLUID      = 47e20000-00000000
        deviceNodeMask  = 1
        deviceLUIDValid = true

VkPhysicalDeviceInlineUniformBlockPropertiesEXT:
------------------------------------------------
        maxInlineUniformBlockSize                               = 256
        maxPerStageDescriptorInlineUniformBlocks                = 32
        maxPerStageDescriptorUpdateAfterBindInlineUniformBlocks = 32
        maxDescriptorSetInlineUniformBlocks                     = 32
        maxDescriptorSetUpdateAfterBindInlineUniformBlocks      = 32

VkPhysicalDeviceLineRasterizationPropertiesEXT:
-----------------------------------------------
        lineSubPixelPrecisionBits = 8

VkPhysicalDeviceMaintenance3Properties:
---------------------------------------
        maxPerSetDescriptors    = 4294967295
        maxMemoryAllocationSize = 0xffe00000

VkPhysicalDeviceMultiviewProperties:
------------------------------------
        maxMultiviewViewCount     = 32
        maxMultiviewInstanceIndex = 134217727

VkPhysicalDevicePCIBusInfoPropertiesEXT:
----------------------------------------
        pciDomain   = 0
        pciBus      = 8
        pciDevice   = 0
        pciFunction = 0

VkPhysicalDevicePointClippingProperties:
----------------------------------------
        pointClippingBehavior = POINT_CLIPPING_BEHAVIOR_USER_CLIP_PLANES_ONLY

VkPhysicalDeviceProtectedMemoryProperties:
------------------------------------------
        protectedNoFault = false

VkPhysicalDevicePushDescriptorPropertiesKHR:
--------------------------------------------
        maxPushDescriptors = 32

VkPhysicalDeviceRayTracingPipelinePropertiesKHR:
------------------------------------------------
        shaderGroupHandleSize              = 32
        maxRayRecursionDepth               = 31
        maxShaderGroupStride               = 4096
        shaderGroupBaseAlignment           = 64
        shaderGroupHandleCaptureReplaySize = 32
        maxRayDispatchInvocationCount      = 1073741824
        shaderGroupHandleAlignment         = 32
        maxRayHitAttributeSize             = 32

VkPhysicalDeviceRobustness2PropertiesEXT:
-----------------------------------------
        robustStorageBufferAccessSizeAlignment = 0x00000001
        robustUniformBufferAccessSizeAlignment = 0x00000010

VkPhysicalDeviceSampleLocationsPropertiesEXT:
---------------------------------------------
        sampleLocationSampleCounts: count = 5
                SAMPLE_COUNT_1_BIT
                SAMPLE_COUNT_2_BIT
                SAMPLE_COUNT_4_BIT
                SAMPLE_COUNT_8_BIT
                SAMPLE_COUNT_16_BIT
        maxSampleLocationGridSize:
                width  = 1
                height = 1
        sampleLocationCoordinateRange: count = 2
                0
                0.9375
        sampleLocationSubPixelBits       = 4
        variableSampleLocations          = true

VkPhysicalDeviceSamplerFilterMinmaxProperties:
----------------------------------------------
        filterMinmaxSingleComponentFormats = true
        filterMinmaxImageComponentMapping  = true

VkPhysicalDeviceSubgroupProperties:
-----------------------------------
        subgroupSize              = 32
        supportedStages: count = 16
                SHADER_STAGE_VERTEX_BIT
                SHADER_STAGE_TESSELLATION_CONTROL_BIT
                SHADER_STAGE_TESSELLATION_EVALUATION_BIT
                SHADER_STAGE_GEOMETRY_BIT
                SHADER_STAGE_FRAGMENT_BIT
                SHADER_STAGE_COMPUTE_BIT
                SHADER_STAGE_ALL_GRAPHICS
                SHADER_STAGE_ALL
                SHADER_STAGE_RAYGEN_BIT_KHR
                SHADER_STAGE_ANY_HIT_BIT_KHR
                SHADER_STAGE_CLOSEST_HIT_BIT_KHR
                SHADER_STAGE_MISS_BIT_KHR
                SHADER_STAGE_INTERSECTION_BIT_KHR
                SHADER_STAGE_CALLABLE_BIT_KHR
                SHADER_STAGE_TASK_BIT_NV
                SHADER_STAGE_MESH_BIT_NV
        supportedOperations: count = 9
                SUBGROUP_FEATURE_BASIC_BIT
                SUBGROUP_FEATURE_VOTE_BIT
                SUBGROUP_FEATURE_ARITHMETIC_BIT
                SUBGROUP_FEATURE_BALLOT_BIT
                SUBGROUP_FEATURE_SHUFFLE_BIT
                SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT
                SUBGROUP_FEATURE_CLUSTERED_BIT
                SUBGROUP_FEATURE_QUAD_BIT
                SUBGROUP_FEATURE_PARTITIONED_BIT_NV
        quadOperationsInAllStages = true

VkPhysicalDeviceSubgroupSizeControlPropertiesEXT:
-------------------------------------------------
        minSubgroupSize              = 32
        maxSubgroupSize              = 32
        maxComputeWorkgroupSubgroups = 2097152
        requiredSubgroupSizeStages: count = 16
                SHADER_STAGE_VERTEX_BIT
                SHADER_STAGE_TESSELLATION_CONTROL_BIT
                SHADER_STAGE_TESSELLATION_EVALUATION_BIT
                SHADER_STAGE_GEOMETRY_BIT
                SHADER_STAGE_FRAGMENT_BIT
                SHADER_STAGE_COMPUTE_BIT
                SHADER_STAGE_ALL_GRAPHICS
                SHADER_STAGE_ALL
                SHADER_STAGE_RAYGEN_BIT_KHR
                SHADER_STAGE_ANY_HIT_BIT_KHR
                SHADER_STAGE_CLOSEST_HIT_BIT_KHR
                SHADER_STAGE_MISS_BIT_KHR
                SHADER_STAGE_INTERSECTION_BIT_KHR
                SHADER_STAGE_CALLABLE_BIT_KHR
                SHADER_STAGE_TASK_BIT_NV
                SHADER_STAGE_MESH_BIT_NV

VkPhysicalDeviceTexelBufferAlignmentPropertiesEXT:
--------------------------------------------------
        storageTexelBufferOffsetAlignmentBytes       = 0x00000010
        storageTexelBufferOffsetSingleTexelAlignment = true
        uniformTexelBufferOffsetAlignmentBytes       = 0x00000010
        uniformTexelBufferOffsetSingleTexelAlignment = true

VkPhysicalDeviceTimelineSemaphoreProperties:
--------------------------------------------
        maxTimelineSemaphoreValueDifference = 2147483647

VkPhysicalDeviceTransformFeedbackPropertiesEXT:
-----------------------------------------------
        maxTransformFeedbackStreams                = 4
        maxTransformFeedbackBuffers                = 4
        maxTransformFeedbackBufferSize             = 0xffffffffffffffff
        maxTransformFeedbackStreamDataSize         = 2048
        maxTransformFeedbackBufferDataSize         = 512
        maxTransformFeedbackBufferDataStride       = 2048
        transformFeedbackQueries                   = true
        transformFeedbackStreamsLinesTriangles     = false
        transformFeedbackRasterizationStreamSelect = true
        transformFeedbackDraw                      = true

VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT:
----------------------------------------------------
        maxVertexAttribDivisor = 4294967295

VkPhysicalDeviceVulkan11Properties:
-----------------------------------
        deviceUUID                        = 09677296-49dc-ce27-f332-7de5c453bfea
        driverUUID                        = d1a0dc91-7a44-affa-d760-3617b3a9d3d9
        deviceLUID                        = 47e20000-00000000
        deviceNodeMask                    = 1
        deviceLUIDValid                   = true
        subgroupSize                      = 32
        subgroupSupportedStages: count = 16
                SHADER_STAGE_VERTEX_BIT
                SHADER_STAGE_TESSELLATION_CONTROL_BIT
                SHADER_STAGE_TESSELLATION_EVALUATION_BIT
                SHADER_STAGE_GEOMETRY_BIT
                SHADER_STAGE_FRAGMENT_BIT
                SHADER_STAGE_COMPUTE_BIT
                SHADER_STAGE_ALL_GRAPHICS
                SHADER_STAGE_ALL
                SHADER_STAGE_RAYGEN_BIT_KHR
                SHADER_STAGE_ANY_HIT_BIT_KHR
                SHADER_STAGE_CLOSEST_HIT_BIT_KHR
                SHADER_STAGE_MISS_BIT_KHR
                SHADER_STAGE_INTERSECTION_BIT_KHR
                SHADER_STAGE_CALLABLE_BIT_KHR
                SHADER_STAGE_TASK_BIT_NV
                SHADER_STAGE_MESH_BIT_NV
        subgroupSupportedOperations: count = 9
                SUBGROUP_FEATURE_BASIC_BIT
                SUBGROUP_FEATURE_VOTE_BIT
                SUBGROUP_FEATURE_ARITHMETIC_BIT
                SUBGROUP_FEATURE_BALLOT_BIT
                SUBGROUP_FEATURE_SHUFFLE_BIT
                SUBGROUP_FEATURE_SHUFFLE_RELATIVE_BIT
                SUBGROUP_FEATURE_CLUSTERED_BIT
                SUBGROUP_FEATURE_QUAD_BIT
                SUBGROUP_FEATURE_PARTITIONED_BIT_NV
        subgroupQuadOperationsInAllStages = true
        pointClippingBehavior             = POINT_CLIPPING_BEHAVIOR_USER_CLIP_PLANES_ONLY
        maxMultiviewViewCount             = 32
        maxMultiviewInstanceIndex         = 134217727
        protectedNoFault                  = false
        maxPerSetDescriptors              = 4294967295
        maxMemoryAllocationSize           = 0xffe00000

VkPhysicalDeviceVulkan12Properties:
-----------------------------------
        driverID                                             = DRIVER_ID_NVIDIA_PROPRIETARY
        driverName                                           = NVIDIA
        driverInfo                                           = 471.68
        conformanceVersion                                   = 1.2.6.0
        denormBehaviorIndependence                           = SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL
        roundingModeIndependence                             = SHADER_FLOAT_CONTROLS_INDEPENDENCE_ALL
        shaderSignedZeroInfNanPreserveFloat16                = true
        shaderSignedZeroInfNanPreserveFloat32                = true
        shaderSignedZeroInfNanPreserveFloat64                = true
        shaderDenormPreserveFloat16                          = true
        shaderDenormPreserveFloat32                          = false
        shaderDenormPreserveFloat64                          = false
        shaderDenormFlushToZeroFloat16                       = false
        shaderDenormFlushToZeroFloat32                       = false
        shaderDenormFlushToZeroFloat64                       = false
        shaderRoundingModeRTEFloat16                         = true
        shaderRoundingModeRTEFloat32                         = true
        shaderRoundingModeRTEFloat64                         = true
        shaderRoundingModeRTZFloat16                         = false
        shaderRoundingModeRTZFloat32                         = true
        shaderRoundingModeRTZFloat64                         = true
        maxUpdateAfterBindDescriptorsInAllPools              = 4294967295
        shaderUniformBufferArrayNonUniformIndexingNative     = true
        shaderSampledImageArrayNonUniformIndexingNative      = true
        shaderStorageBufferArrayNonUniformIndexingNative     = true
        shaderStorageImageArrayNonUniformIndexingNative      = true
        shaderInputAttachmentArrayNonUniformIndexingNative   = true
        robustBufferAccessUpdateAfterBind                    = true
        quadDivergentImplicitLod                             = true
        maxPerStageDescriptorUpdateAfterBindSamplers         = 1048576
        maxPerStageDescriptorUpdateAfterBindUniformBuffers   = 1048576
        maxPerStageDescriptorUpdateAfterBindStorageBuffers   = 1048576
        maxPerStageDescriptorUpdateAfterBindSampledImages    = 1048576
        maxPerStageDescriptorUpdateAfterBindStorageImages    = 1048576
        maxPerStageDescriptorUpdateAfterBindInputAttachments = 1048576
        maxPerStageUpdateAfterBindResources                  = 4294967295
        maxDescriptorSetUpdateAfterBindSamplers              = 1048576
        maxDescriptorSetUpdateAfterBindUniformBuffers        = 1048576
        maxDescriptorSetUpdateAfterBindUniformBuffersDynamic = 15
        maxDescriptorSetUpdateAfterBindStorageBuffers        = 1048576
        maxDescriptorSetUpdateAfterBindStorageBuffersDynamic = 16
        maxDescriptorSetUpdateAfterBindSampledImages         = 1048576
        maxDescriptorSetUpdateAfterBindStorageImages         = 1048576
        maxDescriptorSetUpdateAfterBindInputAttachments      = 1048576
        supportedDepthResolveModes: count = 4
                RESOLVE_MODE_SAMPLE_ZERO_BIT
                RESOLVE_MODE_AVERAGE_BIT
                RESOLVE_MODE_MIN_BIT
                RESOLVE_MODE_MAX_BIT
        supportedStencilResolveModes: count = 3
                RESOLVE_MODE_SAMPLE_ZERO_BIT
                RESOLVE_MODE_MIN_BIT
                RESOLVE_MODE_MAX_BIT
        independentResolveNone                               = true
        independentResolve                                   = true
        filterMinmaxSingleComponentFormats                   = true
        filterMinmaxImageComponentMapping                    = true
        maxTimelineSemaphoreValueDifference                  = 2147483647
        framebufferIntegerColorSampleCounts: count = 4
                SAMPLE_COUNT_1_BIT
                SAMPLE_COUNT_2_BIT
                SAMPLE_COUNT_4_BIT
                SAMPLE_COUNT_8_BIT


Device Extensions: count = 141
        VK_EXT_4444_formats                       : extension revision 1
        VK_EXT_blend_operation_advanced           : extension revision 2
        VK_EXT_buffer_device_address              : extension revision 2
        VK_EXT_calibrated_timestamps              : extension revision 1
        VK_EXT_color_write_enable                 : extension revision 1
        VK_EXT_conditional_rendering              : extension revision 2
        VK_EXT_conservative_rasterization         : extension revision 1
        VK_EXT_custom_border_color                : extension revision 12
        VK_EXT_depth_clip_enable                  : extension revision 1
        VK_EXT_depth_range_unrestricted           : extension revision 1
        VK_EXT_descriptor_indexing                : extension revision 2
        VK_EXT_discard_rectangles                 : extension revision 1
        VK_EXT_extended_dynamic_state             : extension revision 1
        VK_EXT_extended_dynamic_state2            : extension revision 1
        VK_EXT_external_memory_host               : extension revision 1
        VK_EXT_fragment_shader_interlock          : extension revision 1
        VK_EXT_full_screen_exclusive              : extension revision 4
        VK_EXT_hdr_metadata                       : extension revision 2
        VK_EXT_host_query_reset                   : extension revision 1
        VK_EXT_image_robustness                   : extension revision 1
        VK_EXT_index_type_uint8                   : extension revision 1
        VK_EXT_inline_uniform_block               : extension revision 1
        VK_EXT_line_rasterization                 : extension revision 1
        VK_EXT_memory_budget                      : extension revision 1
        VK_EXT_memory_priority                    : extension revision 1
        VK_EXT_pci_bus_info                       : extension revision 2
        VK_EXT_pipeline_creation_cache_control    : extension revision 3
        VK_EXT_pipeline_creation_feedback         : extension revision 1
        VK_EXT_post_depth_coverage                : extension revision 1
        VK_EXT_private_data                       : extension revision 1
        VK_EXT_provoking_vertex                   : extension revision 1
        VK_EXT_queue_family_foreign               : extension revision 1
        VK_EXT_robustness2                        : extension revision 1
        VK_EXT_sample_locations                   : extension revision 1
        VK_EXT_sampler_filter_minmax              : extension revision 2
        VK_EXT_scalar_block_layout                : extension revision 1
        VK_EXT_separate_stencil_usage             : extension revision 1
        VK_EXT_shader_atomic_float                : extension revision 1
        VK_EXT_shader_demote_to_helper_invocation : extension revision 1
        VK_EXT_shader_image_atomic_int64          : extension revision 1
        VK_EXT_shader_subgroup_ballot             : extension revision 1
        VK_EXT_shader_subgroup_vote               : extension revision 1
        VK_EXT_shader_viewport_index_layer        : extension revision 1
        VK_EXT_subgroup_size_control              : extension revision 2
        VK_EXT_texel_buffer_alignment             : extension revision 1
        VK_EXT_tooling_info                       : extension revision 1
        VK_EXT_transform_feedback                 : extension revision 1
        VK_EXT_vertex_attribute_divisor           : extension revision 3
        VK_EXT_vertex_input_dynamic_state         : extension revision 2
        VK_EXT_ycbcr_image_arrays                 : extension revision 1
        VK_KHR_16bit_storage                      : extension revision 1
        VK_KHR_8bit_storage                       : extension revision 1
        VK_KHR_acceleration_structure             : extension revision 11
        VK_KHR_bind_memory2                       : extension revision 1
        VK_KHR_buffer_device_address              : extension revision 1
        VK_KHR_copy_commands2                     : extension revision 1
        VK_KHR_create_renderpass2                 : extension revision 1
        VK_KHR_dedicated_allocation               : extension revision 3
        VK_KHR_deferred_host_operations           : extension revision 4
        VK_KHR_depth_stencil_resolve              : extension revision 1
        VK_KHR_descriptor_update_template         : extension revision 1
        VK_KHR_device_group                       : extension revision 4
        VK_KHR_draw_indirect_count                : extension revision 1
        VK_KHR_driver_properties                  : extension revision 1
        VK_KHR_external_fence                     : extension revision 1
        VK_KHR_external_fence_win32               : extension revision 1
        VK_KHR_external_memory                    : extension revision 1
        VK_KHR_external_memory_win32              : extension revision 1
        VK_KHR_external_semaphore                 : extension revision 1
        VK_KHR_external_semaphore_win32           : extension revision 1
        VK_KHR_fragment_shading_rate              : extension revision 1
        VK_KHR_get_memory_requirements2           : extension revision 1
        VK_KHR_image_format_list                  : extension revision 1
        VK_KHR_imageless_framebuffer              : extension revision 1
        VK_KHR_maintenance1                       : extension revision 2
        VK_KHR_maintenance2                       : extension revision 1
        VK_KHR_maintenance3                       : extension revision 1
        VK_KHR_multiview                          : extension revision 1
        VK_KHR_pipeline_executable_properties     : extension revision 1
        VK_KHR_pipeline_library                   : extension revision 1
        VK_KHR_push_descriptor                    : extension revision 2
        VK_KHR_ray_tracing_pipeline               : extension revision 1
        VK_KHR_relaxed_block_layout               : extension revision 1
        VK_KHR_sampler_mirror_clamp_to_edge       : extension revision 3
        VK_KHR_sampler_ycbcr_conversion           : extension revision 14
        VK_KHR_separate_depth_stencil_layouts     : extension revision 1
        VK_KHR_shader_atomic_int64                : extension revision 1
        VK_KHR_shader_clock                       : extension revision 1
        VK_KHR_shader_draw_parameters             : extension revision 1
        VK_KHR_shader_float16_int8                : extension revision 1
        VK_KHR_shader_float_controls              : extension revision 4
        VK_KHR_shader_non_semantic_info           : extension revision 1
        VK_KHR_shader_subgroup_extended_types     : extension revision 1
        VK_KHR_shader_terminate_invocation        : extension revision 1
        VK_KHR_spirv_1_4                          : extension revision 1
        VK_KHR_storage_buffer_storage_class       : extension revision 1
        VK_KHR_swapchain                          : extension revision 70
        VK_KHR_swapchain_mutable_format           : extension revision 1
        VK_KHR_synchronization2                   : extension revision 1
        VK_KHR_timeline_semaphore                 : extension revision 2
        VK_KHR_uniform_buffer_standard_layout     : extension revision 1
        VK_KHR_variable_pointers                  : extension revision 1
        VK_KHR_vulkan_memory_model                : extension revision 3
        VK_KHR_win32_keyed_mutex                  : extension revision 1
        VK_KHR_workgroup_memory_explicit_layout   : extension revision 1
        VK_KHR_zero_initialize_workgroup_memory   : extension revision 1
        VK_NVX_binary_import                      : extension revision 1
        VK_NVX_image_view_handle                  : extension revision 2
        VK_NVX_multiview_per_view_attributes      : extension revision 1
        VK_NV_acquire_winrt_display               : extension revision 1
        VK_NV_clip_space_w_scaling                : extension revision 1
        VK_NV_compute_shader_derivatives          : extension revision 1
        VK_NV_corner_sampled_image                : extension revision 2
        VK_NV_coverage_reduction_mode             : extension revision 1
        VK_NV_cuda_kernel_launch                  : extension revision 2
        VK_NV_dedicated_allocation                : extension revision 1
        VK_NV_dedicated_allocation_image_aliasing : extension revision 1
        VK_NV_device_diagnostic_checkpoints       : extension revision 2
        VK_NV_device_diagnostics_config           : extension revision 1
        VK_NV_device_generated_commands           : extension revision 3
        VK_NV_external_memory                     : extension revision 1
        VK_NV_external_memory_win32               : extension revision 1
        VK_NV_fill_rectangle                      : extension revision 1
        VK_NV_fragment_coverage_to_color          : extension revision 1
        VK_NV_fragment_shader_barycentric         : extension revision 1
        VK_NV_fragment_shading_rate_enums         : extension revision 1
        VK_NV_framebuffer_mixed_samples           : extension revision 1
        VK_NV_geometry_shader_passthrough         : extension revision 1
        VK_NV_inherited_viewport_scissor          : extension revision 1
        VK_NV_mesh_shader                         : extension revision 1
        VK_NV_ray_tracing                         : extension revision 3
        VK_NV_representative_fragment_test        : extension revision 2
        VK_NV_sample_mask_override_coverage       : extension revision 1
        VK_NV_scissor_exclusive                   : extension revision 1
        VK_NV_shader_image_footprint              : extension revision 2
        VK_NV_shader_sm_builtins                  : extension revision 1
        VK_NV_shader_subgroup_partitioned         : extension revision 1
        VK_NV_shading_rate_image                  : extension revision 3
        VK_NV_viewport_array2                     : extension revision 1
        VK_NV_viewport_swizzle                    : extension revision 1
        VK_NV_win32_keyed_mutex                   : extension revision 2

VkQueueFamilyProperties:
========================
        queueProperties[0]:
        -------------------
                minImageTransferGranularity = (1,1,1)
                queueCount                  = 16
                queueFlags                  = QUEUE_GRAPHICS | QUEUE_COMPUTE | QUEUE_TRANSFER | QUEUE_SPARSE_BINDING
                timestampValidBits          = 64
                present support             = true

        queueProperties[1]:
        -------------------
                minImageTransferGranularity = (1,1,1)
                queueCount                  = 2
                queueFlags                  = QUEUE_TRANSFER | QUEUE_SPARSE_BINDING
                timestampValidBits          = 64
                present support             = false

        queueProperties[2]:
        -------------------
                minImageTransferGranularity = (1,1,1)
                queueCount                  = 8
                queueFlags                  = QUEUE_COMPUTE | QUEUE_TRANSFER | QUEUE_SPARSE_BINDING
                timestampValidBits          = 64
                present support             = true

VkPhysicalDeviceMemoryProperties:
=================================
memoryHeaps: count = 3
        memoryHeaps[0]:
                size   = 6282018816 (0x176700000) (5.85 GiB)
                budget = 5476712448 (0x146700000) (5.10 GiB)
                usage  = 0 (0x00000000) (0.00 B)
                flags: count = 1
                        MEMORY_HEAP_DEVICE_LOCAL_BIT
        memoryHeaps[1]:
                size   = 8543563776 (0x1fd3c7000) (7.96 GiB)
                budget = 7847952077 (0x1d3c63ecd) (7.31 GiB)
                usage  = 278528 (0x00044000) (272.00 KiB)
                flags: count = 0
                        None
        memoryHeaps[2]:
                size   = 224395264 (0x0d600000) (214.00 MiB)
                budget = 218329088 (0x0d037000) (208.21 MiB)
                usage  = 6066176 (0x005c9000) (5.79 MiB)
                flags: count = 1
                        MEMORY_HEAP_DEVICE_LOCAL_BIT
memoryTypes: count = 5
        memoryTypes[0]:
                heapIndex     = 1
                propertyFlags = 0x0000: count = 0
                        None
                usable for:
                        IMAGE_TILING_OPTIMAL:
                                color images
                                FORMAT_D16_UNORM
                                FORMAT_X8_D24_UNORM_PACK32
                                FORMAT_D32_SFLOAT
                                FORMAT_S8_UINT
                                FORMAT_D24_UNORM_S8_UINT
                                FORMAT_D32_SFLOAT_S8_UINT
                                (non-sparse)
                        IMAGE_TILING_LINEAR:
                                color images
                                (non-sparse, non-transient)
        memoryTypes[1]:
                heapIndex     = 0
                propertyFlags = 0x0001: count = 1
                        MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                usable for:
                        IMAGE_TILING_OPTIMAL:
                                color images
                                FORMAT_D16_UNORM
                                FORMAT_X8_D24_UNORM_PACK32
                                FORMAT_D32_SFLOAT
                                FORMAT_S8_UINT
                                FORMAT_D24_UNORM_S8_UINT
                                FORMAT_D32_SFLOAT_S8_UINT
                                (non-sparse)
                        IMAGE_TILING_LINEAR:
                                color images
                                (non-sparse, non-transient)
        memoryTypes[2]:
                heapIndex     = 1
                propertyFlags = 0x0006: count = 2
                        MEMORY_PROPERTY_HOST_VISIBLE_BIT
                        MEMORY_PROPERTY_HOST_COHERENT_BIT
                usable for:
                        IMAGE_TILING_OPTIMAL:
                                None
                        IMAGE_TILING_LINEAR:
                                color images
                                (non-sparse, non-transient)
        memoryTypes[3]:
                heapIndex     = 1
                propertyFlags = 0x000e: count = 3
                        MEMORY_PROPERTY_HOST_VISIBLE_BIT
                        MEMORY_PROPERTY_HOST_COHERENT_BIT
                        MEMORY_PROPERTY_HOST_CACHED_BIT
                usable for:
                        IMAGE_TILING_OPTIMAL:
                                None
                        IMAGE_TILING_LINEAR:
                                color images
                                (non-sparse, non-transient)
        memoryTypes[4]:
                heapIndex     = 2
                propertyFlags = 0x0007: count = 3
                        MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                        MEMORY_PROPERTY_HOST_VISIBLE_BIT
                        MEMORY_PROPERTY_HOST_COHERENT_BIT
                usable for:
                        IMAGE_TILING_OPTIMAL:
                                None
                        IMAGE_TILING_LINEAR:
                                color images
                                (non-sparse, non-transient)

VkPhysicalDeviceFeatures:
=========================
        robustBufferAccess                      = true
        fullDrawIndexUint32                     = true
        imageCubeArray                          = true
        independentBlend                        = true
        geometryShader                          = true
        tessellationShader                      = true
        sampleRateShading                       = true
        dualSrcBlend                            = true
        logicOp                                 = true
        multiDrawIndirect                       = true
        drawIndirectFirstInstance               = true
        depthClamp                              = true
        depthBiasClamp                          = true
        fillModeNonSolid                        = true
        depthBounds                             = true
        wideLines                               = true
        largePoints                             = true
        alphaToOne                              = true
        multiViewport                           = true
        samplerAnisotropy                       = true
        textureCompressionETC2                  = false
        textureCompressionASTC_LDR              = false
        textureCompressionBC                    = true
        occlusionQueryPrecise                   = true
        pipelineStatisticsQuery                 = true
        vertexPipelineStoresAndAtomics          = true
        fragmentStoresAndAtomics                = true
        shaderTessellationAndGeometryPointSize  = true
        shaderImageGatherExtended               = true
        shaderStorageImageExtendedFormats       = true
        shaderStorageImageMultisample           = true
        shaderStorageImageReadWithoutFormat     = true
        shaderStorageImageWriteWithoutFormat    = true
        shaderUniformBufferArrayDynamicIndexing = true
        shaderSampledImageArrayDynamicIndexing  = true
        shaderStorageBufferArrayDynamicIndexing = true
        shaderStorageImageArrayDynamicIndexing  = true
        shaderClipDistance                      = true
        shaderCullDistance                      = true
        shaderFloat64                           = true
        shaderInt64                             = true
        shaderInt16                             = true
        shaderResourceResidency                 = true
        shaderResourceMinLod                    = true
        sparseBinding                           = true
        sparseResidencyBuffer                   = true
        sparseResidencyImage2D                  = true
        sparseResidencyImage3D                  = true
        sparseResidency2Samples                 = true
        sparseResidency4Samples                 = true
        sparseResidency8Samples                 = true
        sparseResidency16Samples                = true
        sparseResidencyAliased                  = true
        variableMultisampleRate                 = true
        inheritedQueries                        = true

VkPhysicalDevice16BitStorageFeatures:
-------------------------------------
        storageBuffer16BitAccess           = true
        uniformAndStorageBuffer16BitAccess = true
        storagePushConstant16              = true
        storageInputOutput16               = false

VkPhysicalDevice4444FormatsFeaturesEXT:
---------------------------------------
        formatA4R4G4B4 = true
        formatA4B4G4R4 = true

VkPhysicalDevice8BitStorageFeatures:
------------------------------------
        storageBuffer8BitAccess           = true
        uniformAndStorageBuffer8BitAccess = true
        storagePushConstant8              = true

VkPhysicalDeviceAccelerationStructureFeaturesKHR:
-------------------------------------------------
        accelerationStructure                                 = true
        accelerationStructureCaptureReplay                    = true
        accelerationStructureIndirectBuild                    = false
        accelerationStructureHostCommands                     = false
        descriptorBindingAccelerationStructureUpdateAfterBind = true

VkPhysicalDeviceBlendOperationAdvancedFeaturesEXT:
--------------------------------------------------
        advancedBlendCoherentOperations = true

VkPhysicalDeviceBufferDeviceAddressFeatures:
--------------------------------------------
        bufferDeviceAddress              = true
        bufferDeviceAddressCaptureReplay = true
        bufferDeviceAddressMultiDevice   = true

VkPhysicalDeviceBufferDeviceAddressFeaturesEXT:
-----------------------------------------------
        bufferDeviceAddress              = true
        bufferDeviceAddressCaptureReplay = true
        bufferDeviceAddressMultiDevice   = true

VkPhysicalDeviceConditionalRenderingFeaturesEXT:
------------------------------------------------
        conditionalRendering          = true
        inheritedConditionalRendering = true

VkPhysicalDeviceCustomBorderColorFeaturesEXT:
---------------------------------------------
        customBorderColors             = true
        customBorderColorWithoutFormat = true

VkPhysicalDeviceDepthClipEnableFeaturesEXT:
-------------------------------------------
        depthClipEnable = true

VkPhysicalDeviceDescriptorIndexingFeatures:
-------------------------------------------
        shaderInputAttachmentArrayDynamicIndexing          = true
        shaderUniformTexelBufferArrayDynamicIndexing       = true
        shaderStorageTexelBufferArrayDynamicIndexing       = true
        shaderUniformBufferArrayNonUniformIndexing         = true
        shaderSampledImageArrayNonUniformIndexing          = true
        shaderStorageBufferArrayNonUniformIndexing         = true
        shaderStorageImageArrayNonUniformIndexing          = true
        shaderInputAttachmentArrayNonUniformIndexing       = true
        shaderUniformTexelBufferArrayNonUniformIndexing    = true
        shaderStorageTexelBufferArrayNonUniformIndexing    = true
        descriptorBindingUniformBufferUpdateAfterBind      = true
        descriptorBindingSampledImageUpdateAfterBind       = true
        descriptorBindingStorageImageUpdateAfterBind       = true
        descriptorBindingStorageBufferUpdateAfterBind      = true
        descriptorBindingUniformTexelBufferUpdateAfterBind = true
        descriptorBindingStorageTexelBufferUpdateAfterBind = true
        descriptorBindingUpdateUnusedWhilePending          = true
        descriptorBindingPartiallyBound                    = true
        descriptorBindingVariableDescriptorCount           = true
        runtimeDescriptorArray                             = true

VkPhysicalDeviceExtendedDynamicStateFeaturesEXT:
------------------------------------------------
        extendedDynamicState = true

VkPhysicalDeviceFragmentShaderInterlockFeaturesEXT:
---------------------------------------------------
        fragmentShaderSampleInterlock      = true
        fragmentShaderPixelInterlock       = true
        fragmentShaderShadingRateInterlock = true

VkPhysicalDeviceFragmentShadingRateFeaturesKHR:
-----------------------------------------------
        pipelineFragmentShadingRate   = true
        primitiveFragmentShadingRate  = true
        attachmentFragmentShadingRate = true

VkPhysicalDeviceHostQueryResetFeatures:
---------------------------------------
        hostQueryReset = true

VkPhysicalDeviceImageRobustnessFeaturesEXT:
-------------------------------------------
        robustImageAccess = true

VkPhysicalDeviceImagelessFramebufferFeatures:
---------------------------------------------
        imagelessFramebuffer = true

VkPhysicalDeviceIndexTypeUint8FeaturesEXT:
------------------------------------------
        indexTypeUint8 = true

VkPhysicalDeviceInlineUniformBlockFeaturesEXT:
----------------------------------------------
        inlineUniformBlock                                 = true
        descriptorBindingInlineUniformBlockUpdateAfterBind = true

VkPhysicalDeviceLineRasterizationFeaturesEXT:
---------------------------------------------
        rectangularLines         = true
        bresenhamLines           = true
        smoothLines              = true
        stippledRectangularLines = true
        stippledBresenhamLines   = true
        stippledSmoothLines      = true

VkPhysicalDeviceMemoryPriorityFeaturesEXT:
------------------------------------------
        memoryPriority = true

VkPhysicalDeviceMultiviewFeatures:
----------------------------------
        multiview                   = true
        multiviewGeometryShader     = true
        multiviewTessellationShader = true

VkPhysicalDevicePipelineCreationCacheControlFeaturesEXT:
--------------------------------------------------------
        pipelineCreationCacheControl = true

VkPhysicalDevicePipelineExecutablePropertiesFeaturesKHR:
--------------------------------------------------------
        pipelineExecutableInfo = true

VkPhysicalDevicePrivateDataFeaturesEXT:
---------------------------------------
        privateData = true

VkPhysicalDeviceProtectedMemoryFeatures:
----------------------------------------
        protectedMemory = false

VkPhysicalDeviceRayTracingPipelineFeaturesKHR:
----------------------------------------------
        rayTracingPipeline                                    = true
        rayTracingPipelineShaderGroupHandleCaptureReplay      = false
        rayTracingPipelineShaderGroupHandleCaptureReplayMixed = false
        rayTracingPipelineTraceRaysIndirect                   = true
        rayTraversalPrimitiveCulling                          = true

VkPhysicalDeviceRobustness2FeaturesEXT:
---------------------------------------
        robustBufferAccess2 = true
        robustImageAccess2  = true
        nullDescriptor      = true

VkPhysicalDeviceSamplerYcbcrConversionFeatures:
-----------------------------------------------
        samplerYcbcrConversion = true

VkPhysicalDeviceScalarBlockLayoutFeatures:
------------------------------------------
        scalarBlockLayout = true

VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures:
----------------------------------------------------
        separateDepthStencilLayouts = true

VkPhysicalDeviceShaderAtomicFloatFeaturesEXT:
---------------------------------------------
        shaderBufferFloat32Atomics   = true
        shaderBufferFloat32AtomicAdd = true
        shaderBufferFloat64Atomics   = true
        shaderBufferFloat64AtomicAdd = true
        shaderSharedFloat32Atomics   = true
        shaderSharedFloat32AtomicAdd = true
        shaderSharedFloat64Atomics   = true
        shaderSharedFloat64AtomicAdd = true
        shaderImageFloat32Atomics    = true
        shaderImageFloat32AtomicAdd  = true
        sparseImageFloat32Atomics    = true
        sparseImageFloat32AtomicAdd  = true

VkPhysicalDeviceShaderAtomicInt64Features:
------------------------------------------
        shaderBufferInt64Atomics = true
        shaderSharedInt64Atomics = true

VkPhysicalDeviceShaderClockFeaturesKHR:
---------------------------------------
        shaderSubgroupClock = true
        shaderDeviceClock   = true

VkPhysicalDeviceShaderDemoteToHelperInvocationFeaturesEXT:
----------------------------------------------------------
        shaderDemoteToHelperInvocation = true

VkPhysicalDeviceShaderDrawParametersFeatures:
---------------------------------------------
        shaderDrawParameters = true

VkPhysicalDeviceShaderFloat16Int8Features:
------------------------------------------
        shaderFloat16 = true
        shaderInt8    = true

VkPhysicalDeviceShaderImageAtomicInt64FeaturesEXT:
--------------------------------------------------
        shaderImageInt64Atomics = true
        sparseImageInt64Atomics = true

VkPhysicalDeviceShaderSubgroupExtendedTypesFeatures:
----------------------------------------------------
        shaderSubgroupExtendedTypes = true

VkPhysicalDeviceShaderTerminateInvocationFeaturesKHR:
-----------------------------------------------------
        shaderTerminateInvocation = true

VkPhysicalDeviceSubgroupSizeControlFeaturesEXT:
-----------------------------------------------
        subgroupSizeControl  = true
        computeFullSubgroups = true

VkPhysicalDeviceSynchronization2FeaturesKHR:
--------------------------------------------
        synchronization2 = true

VkPhysicalDeviceTexelBufferAlignmentFeaturesEXT:
------------------------------------------------
        texelBufferAlignment = true

VkPhysicalDeviceTimelineSemaphoreFeatures:
------------------------------------------
        timelineSemaphore = true

VkPhysicalDeviceTransformFeedbackFeaturesEXT:
---------------------------------------------
        transformFeedback = true
        geometryStreams   = true

VkPhysicalDeviceUniformBufferStandardLayoutFeatures:
----------------------------------------------------
        uniformBufferStandardLayout = true

VkPhysicalDeviceVariablePointersFeatures:
-----------------------------------------
        variablePointersStorageBuffer = true
        variablePointers              = true

VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT:
--------------------------------------------------
        vertexAttributeInstanceRateDivisor     = true
        vertexAttributeInstanceRateZeroDivisor = true

VkPhysicalDeviceVulkan11Features:
---------------------------------
        storageBuffer16BitAccess           = true
        uniformAndStorageBuffer16BitAccess = true
        storagePushConstant16              = true
        storageInputOutput16               = false
        multiview                          = true
        multiviewGeometryShader            = true
        multiviewTessellationShader        = true
        variablePointersStorageBuffer      = true
        variablePointers                   = true
        protectedMemory                    = false
        samplerYcbcrConversion             = true
        shaderDrawParameters               = true

VkPhysicalDeviceVulkan12Features:
---------------------------------
        samplerMirrorClampToEdge                           = true
        drawIndirectCount                                  = true
        storageBuffer8BitAccess                            = true
        uniformAndStorageBuffer8BitAccess                  = true
        storagePushConstant8                               = true
        shaderBufferInt64Atomics                           = true
        shaderSharedInt64Atomics                           = true
        shaderFloat16                                      = true
        shaderInt8                                         = true
        descriptorIndexing                                 = true
        shaderInputAttachmentArrayDynamicIndexing          = true
        shaderUniformTexelBufferArrayDynamicIndexing       = true
        shaderStorageTexelBufferArrayDynamicIndexing       = true
        shaderUniformBufferArrayNonUniformIndexing         = true
        shaderSampledImageArrayNonUniformIndexing          = true
        shaderStorageBufferArrayNonUniformIndexing         = true
        shaderStorageImageArrayNonUniformIndexing          = true
        shaderInputAttachmentArrayNonUniformIndexing       = true
        shaderUniformTexelBufferArrayNonUniformIndexing    = true
        shaderStorageTexelBufferArrayNonUniformIndexing    = true
        descriptorBindingUniformBufferUpdateAfterBind      = true
        descriptorBindingSampledImageUpdateAfterBind       = true
        descriptorBindingStorageImageUpdateAfterBind       = true
        descriptorBindingStorageBufferUpdateAfterBind      = true
        descriptorBindingUniformTexelBufferUpdateAfterBind = true
        descriptorBindingStorageTexelBufferUpdateAfterBind = true
        descriptorBindingUpdateUnusedWhilePending          = true
        descriptorBindingPartiallyBound                    = true
        descriptorBindingVariableDescriptorCount           = true
        runtimeDescriptorArray                             = true
        samplerFilterMinmax                                = true
        scalarBlockLayout                                  = true
        imagelessFramebuffer                               = true
        uniformBufferStandardLayout                        = true
        shaderSubgroupExtendedTypes                        = true
        separateDepthStencilLayouts                        = true
        hostQueryReset                                     = true
        timelineSemaphore                                  = true
        bufferDeviceAddress                                = true
        bufferDeviceAddressCaptureReplay                   = true
        bufferDeviceAddressMultiDevice                     = true
        vulkanMemoryModel                                  = true
        vulkanMemoryModelDeviceScope                       = true
        vulkanMemoryModelAvailabilityVisibilityChains      = true
        shaderOutputViewportIndex                          = true
        shaderOutputLayer                                  = true
        subgroupBroadcastDynamicId                         = true

VkPhysicalDeviceVulkanMemoryModelFeatures:
------------------------------------------
        vulkanMemoryModel                             = true
        vulkanMemoryModelDeviceScope                  = true
        vulkanMemoryModelAvailabilityVisibilityChains = true

VkPhysicalDeviceWorkgroupMemoryExplicitLayoutFeaturesKHR:
---------------------------------------------------------
        workgroupMemoryExplicitLayout                  = true
        workgroupMemoryExplicitLayoutScalarBlockLayout = true
        workgroupMemoryExplicitLayout8BitAccess        = true
        workgroupMemoryExplicitLayout16BitAccess       = true

VkPhysicalDeviceYcbcrImageArraysFeaturesEXT:
--------------------------------------------
        ycbcrImageArrays = true

VkPhysicalDeviceZeroInitializeWorkgroupMemoryFeaturesKHR:
---------------------------------------------------------
        shaderZeroInitializeWorkgroupMemory = true*/