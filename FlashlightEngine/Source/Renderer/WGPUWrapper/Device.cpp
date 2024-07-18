#include "FlashlightEngine/Renderer/WGPUWrapper/Device.hpp"

#include "FlashlightEngine/Core/Logger.hpp"

#include "FlashlightEngine/pch.hpp"

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

        Log::EngineTrace("Releasing WebGPU adapter.");
        wgpuAdapterRelease(adapter);
    }

    void Device::Destroy() const {
        /*if (m_Device) {
            // wgpuDeviceRelease(m_Device);
        }*/
    }

    /**
     * @brief Utility function to get a WebGPU Adapter.
     * @param instance The WebGPU instance used by the application.
     * @param options Options for the request.
     * @return A WebGPU adapter.
     */
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
        auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const *pMessage, void *pUserData) {
            auto & [Adapter, RequestEnded] = *static_cast<UserData*>(pUserData);

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

}
