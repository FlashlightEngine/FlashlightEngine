/* Copyright (C) 2024 Jean "Pixfri" Letessier
 * This file is part of "Flashlight Engine"
 * For conditions of distribution and use, see copyright notice in LICENSE.
 *
 * File : Device.cpp
 * Description : Definitions of methods from the Device class.
 */
#include "FlashlightEngine/Renderer/WGPUWrapper/Device.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

#include "FlashlightEngine/pch.hpp"

#include <magic_enum.hpp>

namespace Flashlight::WGPUWrapper {
    Device::Device(const Instance& instance) {
        Create(instance.GetNativeInstance());
    }

    Device::~Device() {
        Destroy();
    }


    void Device::Create(WGPUInstance instance) {
        Log::EngineTrace("Requesting WebGPU adapter...");

        WGPURequestAdapterOptions adapterOptions = {};
        adapterOptions.nextInChain = nullptr;

        WGPUAdapter adapter = RequestAdapterSync(instance, &adapterOptions);

        Log::EngineTrace("Got WebGPU adapter.");

        InspectAdapter(adapter);

        Log::EngineTrace("Requesting WebGPU device...");

        WGPUDeviceDescriptor deviceDescriptor = {};

        deviceDescriptor.nextInChain = nullptr;
        deviceDescriptor.label = "Flashlight Engine WebGPU device";
        deviceDescriptor.requiredFeatureCount = 0;
        deviceDescriptor.requiredLimits = nullptr;
        deviceDescriptor.defaultQueue.nextInChain = nullptr;
        deviceDescriptor.defaultQueue.label = "Flashlight Engine WebGPU queue";
        deviceDescriptor.deviceLostCallback = [](WGPUDeviceLostReason reason, char const* pMessage, void*/*pUserData*/) {
            Log::EngineInfo("Device lost: reason: {0}", magic_enum::enum_name<WGPUDeviceLostReason>(reason));
            if (pMessage) {
                Log::EngineInfo("({0})", pMessage);
            }
        };

        m_Device = RequestDeviceSync(adapter, &deviceDescriptor);

        Log::EngineTrace("Got WebGPU device.");

        auto onDeviceError = [](WGPUErrorType type, char const *pMessage, void */*pUserData*/) {
            Log::EngineError("Uncaptured device error: type: {0}", magic_enum::enum_name<WGPUErrorType>(type));
            if (pMessage) {
                Log::EngineError("({0})", pMessage);
            }
        };
        wgpuDeviceSetUncapturedErrorCallback(m_Device, onDeviceError, nullptr);

        InspectDevice(m_Device);

        Log::EngineTrace("Releasing WebGPU adapter.");
        wgpuAdapterRelease(adapter);
    }

    void Device::Destroy() const {
        if (m_Device) {
            Log::EngineTrace("Releasing WebGPU device.");
            wgpuDeviceRelease(m_Device);
        }
    }

    WGPUAdapter Device::RequestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const* options) {
        // A simple structure holding the local information shared with the
        // onAdapterRequestEnded callback.
        struct UserData {
            WGPUAdapter Adapter = nullptr;
            bool RequestEnded = false;
        };
        UserData userData;

        // Callback called by wgpuInstanceRequestAdapter when the request returns.
        // This is a C++ lambda function, but could be any function defined in the
        // global scope. It must be non-capturing (the brackets [] are empty) so
        // that it behaves like a regular C function pointer, which is what
        // wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
        // is to convey what we want to capture through the pUserData pointer,
        // provided as the last argument of wgpuInstanceRequestAdapter and recieved
        // by the callback as its last argument.
        auto onAdapterRequestEnded = [](const WGPURequestAdapterStatus status, WGPUAdapter adapter,
                                        char const* pMessage,
                                        void* pUserData) {
            auto& [Adapter, RequestEnded] = *static_cast<UserData*>(pUserData);

            if (status == WGPURequestAdapterStatus_Success) {
                Adapter = adapter;
            } else {
                Log::EngineError("Could not get WebGPU adapter. {0}", pMessage);
            }
            RequestEnded = true;
        };

        // Call the WebGPU request adapter procedure.
        wgpuInstanceRequestAdapter(
            instance,
            options,
            onAdapterRequestEnded,
            &userData);

        assert(userData.RequestEnded);

        return userData.Adapter;
    }

    void Device::InspectAdapter(WGPUAdapter adapter) {
        WGPUSupportedLimits supportedLimits = {};
        supportedLimits.nextInChain = nullptr;

        // We're using wgpu-native so wgpuAdapterGetLimits returns a boolean.
        if (wgpuAdapterGetLimits(adapter, &supportedLimits)) {
            Log::EngineTrace("Adapter limits:");
            Log::EngineTrace(" - maxTextureDimension1D: {0}", supportedLimits.limits.maxTextureDimension1D);
            Log::EngineTrace(" - maxTextureDimension2D: {0}", supportedLimits.limits.maxTextureDimension2D);
            Log::EngineTrace(" - maxTextureDimension3D: {0}", supportedLimits.limits.maxTextureDimension3D);
            Log::EngineTrace(" - maxTextureArrayLayers: {0}", supportedLimits.limits.maxTextureArrayLayers);
            Log::EngineTrace(" - maxBindGroups: {0}", supportedLimits.limits.maxBindGroups);
            Log::EngineTrace(" - maxDynamicUniformBuffersPerPipelineLayout: {0}",
                             supportedLimits.limits.maxDynamicUniformBuffersPerPipelineLayout);
            Log::EngineTrace(" - maxDynamicStorageBuffersPerPipelineLayout: {0}",
                             supportedLimits.limits.maxDynamicStorageBuffersPerPipelineLayout);
            Log::EngineTrace(" - maxSampledTexturesPerShaderStage: {0}",
                             supportedLimits.limits.maxSampledTexturesPerShaderStage);
            Log::EngineTrace(" - maxSamplersPerShaderStage: {0}", supportedLimits.limits.maxSamplersPerShaderStage);
            Log::EngineTrace(" - maxStorageBuffersPerShaderStage: {0}",
                             supportedLimits.limits.maxStorageBuffersPerShaderStage);
            Log::EngineTrace(" - maxStorageTexturesPerShaderStage: {0}",
                             supportedLimits.limits.maxStorageTexturesPerShaderStage);
            Log::EngineTrace(" - maxUniformBuffersPerShaderStage: {0}",
                             supportedLimits.limits.maxUniformBuffersPerShaderStage);
            Log::EngineTrace(" - maxUniformBufferBindingSize: {0}", supportedLimits.limits.maxUniformBufferBindingSize);
            Log::EngineTrace(" - maxStorageBufferBindingSize: {0}", supportedLimits.limits.maxStorageBufferBindingSize);
            Log::EngineTrace(" - minUniformBufferOffsetAlignment: {0}",
                             supportedLimits.limits.minUniformBufferOffsetAlignment);
            Log::EngineTrace(" - minStorageBufferOffsetAlignment: {0}",
                             supportedLimits.limits.minStorageBufferOffsetAlignment);
            Log::EngineTrace(" - maxVertexBuffers: {0}", supportedLimits.limits.maxVertexBuffers);
            Log::EngineTrace(" - maxVertexAttributes: {0}", supportedLimits.limits.maxVertexAttributes);
            Log::EngineTrace(" - maxVertexBufferArrayStride: {0}", supportedLimits.limits.maxVertexBufferArrayStride);
            Log::EngineTrace(" - maxInterStageShaderComponents: {0}",
                             supportedLimits.limits.maxInterStageShaderComponents);
            Log::EngineTrace(" - maxComputeWorkgroupStorageSize: {0}",
                             supportedLimits.limits.maxComputeWorkgroupStorageSize);
            Log::EngineTrace(" - maxComputeInvocationsPerWorkgroup: {0}",
                             supportedLimits.limits.maxComputeInvocationsPerWorkgroup);
            Log::EngineTrace(" - maxComputeWorkgroupSizeX: {0}", supportedLimits.limits.maxComputeWorkgroupSizeX);
            Log::EngineTrace(" - maxComputeWorkgroupSizeY: {0}", supportedLimits.limits.maxComputeWorkgroupSizeY);
            Log::EngineTrace(" - maxComputeWorkgroupSizeZ: {0}", supportedLimits.limits.maxComputeWorkgroupSizeZ);
            Log::EngineTrace(" - maxComputeWorkgroupsPerDimension: {0}",
                             supportedLimits.limits.maxComputeWorkgroupsPerDimension);
        }

        std::vector<WGPUFeatureName> features;

        const size featureCount = wgpuAdapterEnumerateFeatures(adapter, nullptr);

        features.resize(featureCount);

        wgpuAdapterEnumerateFeatures(adapter, features.data());

        Log::EngineTrace("Adaper features:");
        for (const auto featureNumber : features) {
            // Check if the feature is in the WGPUFeatureName enum.
            if (featureNumber <= 0x0000000B) {
                Log::EngineTrace(" - {0}", magic_enum::enum_name<WGPUFeatureName>(featureNumber));
            } else {
                // If it's not, then the feature is part of WGPUNativeFeature. This enum isn't supported by magic_enum so
                // we print it as a hex number.
                std::stringstream featureHexNumber;
                featureHexNumber << std::hex << featureNumber;
                Log::EngineTrace(" - wgpu-native feature: 0x{0}", featureHexNumber.str());
            }
        }

        WGPUAdapterProperties properties = {};
        properties.nextInChain = nullptr;
        wgpuAdapterGetProperties(adapter, &properties);

        Log::EngineTrace("Adapter properties:");
        Log::EngineTrace("- Vender ID: {0}", properties.vendorID);
        if (properties.vendorName) {
            Log::EngineTrace("- Vendor name: {0}", properties.vendorName);
        }
        if (properties.architecture) {
            Log::EngineTrace("- Architecture: {0}", properties.architecture);
        }
        Log::EngineTrace("- Device ID: {0}", properties.deviceID);
        if (properties.name) {
            Log::EngineTrace("- Device name: {0}", properties.name);
        }
        if (properties.driverDescription) {
            Log::EngineTrace("- Driver description: {0}", properties.driverDescription);
        }
        Log::EngineTrace("- Adapter type: {0}", magic_enum::enum_name<WGPUAdapterType>(properties.adapterType));
        Log::EngineTrace("- Backend type: {0}", magic_enum::enum_name<WGPUBackendType>(properties.backendType));
    }

    WGPUDevice Device::RequestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor) {
        struct UserData {
            WGPUDevice Device = nullptr;
            bool RequestEnded = false;
        };
        UserData userData;

        auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const* pMessage,
                                       void* pUserData) {
            auto& [Device, RequestEnded] = *static_cast<UserData*>(pUserData);
            if (status == WGPURequestDeviceStatus_Success) {
                Device = device;
            } else {
                Log::EngineError("Could not get WebGPU device: {0}", pMessage);
            }
            RequestEnded = true;
        };

        wgpuAdapterRequestDevice(
            adapter,
            descriptor,
            onDeviceRequestEnded,
            &userData);

        assert(userData.RequestEnded);

        return userData.Device;
    }

    void Device::InspectDevice(WGPUDevice device) {
        WGPUSupportedLimits supportedLimits = {};
        supportedLimits.nextInChain = nullptr;

        // We're using wgpu-native so wgpuAdapterGetLimits returns a boolean.
        if (wgpuDeviceGetLimits(device, &supportedLimits)) {
            Log::EngineTrace("Device limits:");
            Log::EngineTrace(" - maxTextureDimension1D: {0}", supportedLimits.limits.maxTextureDimension1D);
            Log::EngineTrace(" - maxTextureDimension2D: {0}", supportedLimits.limits.maxTextureDimension2D);
            Log::EngineTrace(" - maxTextureDimension3D: {0}", supportedLimits.limits.maxTextureDimension3D);
            Log::EngineTrace(" - maxTextureArrayLayers: {0}", supportedLimits.limits.maxTextureArrayLayers);
            Log::EngineTrace(" - maxBindGroups: {0}", supportedLimits.limits.maxBindGroups);
            Log::EngineTrace(" - maxDynamicUniformBuffersPerPipelineLayout: {0}",
                             supportedLimits.limits.maxDynamicUniformBuffersPerPipelineLayout);
            Log::EngineTrace(" - maxDynamicStorageBuffersPerPipelineLayout: {0}",
                             supportedLimits.limits.maxDynamicStorageBuffersPerPipelineLayout);
            Log::EngineTrace(" - maxSampledTexturesPerShaderStage: {0}",
                             supportedLimits.limits.maxSampledTexturesPerShaderStage);
            Log::EngineTrace(" - maxSamplersPerShaderStage: {0}", supportedLimits.limits.maxSamplersPerShaderStage);
            Log::EngineTrace(" - maxStorageBuffersPerShaderStage: {0}",
                             supportedLimits.limits.maxStorageBuffersPerShaderStage);
            Log::EngineTrace(" - maxStorageTexturesPerShaderStage: {0}",
                             supportedLimits.limits.maxStorageTexturesPerShaderStage);
            Log::EngineTrace(" - maxUniformBuffersPerShaderStage: {0}",
                             supportedLimits.limits.maxUniformBuffersPerShaderStage);
            Log::EngineTrace(" - maxUniformBufferBindingSize: {0}", supportedLimits.limits.maxUniformBufferBindingSize);
            Log::EngineTrace(" - maxStorageBufferBindingSize: {0}", supportedLimits.limits.maxStorageBufferBindingSize);
            Log::EngineTrace(" - minUniformBufferOffsetAlignment: {0}",
                             supportedLimits.limits.minUniformBufferOffsetAlignment);
            Log::EngineTrace(" - minStorageBufferOffsetAlignment: {0}",
                             supportedLimits.limits.minStorageBufferOffsetAlignment);
            Log::EngineTrace(" - maxVertexBuffers: {0}", supportedLimits.limits.maxVertexBuffers);
            Log::EngineTrace(" - maxVertexAttributes: {0}", supportedLimits.limits.maxVertexAttributes);
            Log::EngineTrace(" - maxVertexBufferArrayStride: {0}", supportedLimits.limits.maxVertexBufferArrayStride);
            Log::EngineTrace(" - maxInterStageShaderComponents: {0}",
                             supportedLimits.limits.maxInterStageShaderComponents);
            Log::EngineTrace(" - maxComputeWorkgroupStorageSize: {0}",
                             supportedLimits.limits.maxComputeWorkgroupStorageSize);
            Log::EngineTrace(" - maxComputeInvocationsPerWorkgroup: {0}",
                             supportedLimits.limits.maxComputeInvocationsPerWorkgroup);
            Log::EngineTrace(" - maxComputeWorkgroupSizeX: {0}", supportedLimits.limits.maxComputeWorkgroupSizeX);
            Log::EngineTrace(" - maxComputeWorkgroupSizeY: {0}", supportedLimits.limits.maxComputeWorkgroupSizeY);
            Log::EngineTrace(" - maxComputeWorkgroupSizeZ: {0}", supportedLimits.limits.maxComputeWorkgroupSizeZ);
            Log::EngineTrace(" - maxComputeWorkgroupsPerDimension: {0}",
                             supportedLimits.limits.maxComputeWorkgroupsPerDimension);
        }

        std::vector<WGPUFeatureName> features;

        const size featureCount = wgpuDeviceEnumerateFeatures(device, nullptr);

        features.resize(featureCount);

        wgpuDeviceEnumerateFeatures(device, features.data());

        Log::EngineTrace("Device features:");
        for (const auto featureNumber : features) {
            // Check if the feature is in the WGPUFeatureName enum.
            if (featureNumber <= 0x0000000B) {
                Log::EngineTrace(" - {0}", magic_enum::enum_name<WGPUFeatureName>(featureNumber));
            } else {
                // If it's not, then the feature is part of WGPUNativeFeature. This enum isn't supported by magic_enum so
                // we print it as a hex number.
                std::stringstream featureHexNumber;
                featureHexNumber << std::hex << featureNumber;
                Log::EngineTrace(" - wgpu-native feature: 0x{0}", featureHexNumber.str());
            }
        }
    }
}
