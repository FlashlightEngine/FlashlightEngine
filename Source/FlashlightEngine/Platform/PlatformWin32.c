// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Platform/Platform.h"

// Windows platform layer
#ifdef FL_PLATFORM_WINDOWS

#include "FlashlightEngine/Core/Logger.h"
#include "FlashlightEngine/Core/Input.h"
#include "FlashlightEngine/Core/FlString.h"
#include "FlashlightEngine/Core/Event.h"

#include "FlashlightEngine/Containers/DArray.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanPlatform.h"
#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"

#include <Windows.h>
#include <windowsx.h> // Param input extraction

#include <stdlib.h>

// For surface creation.
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

typedef struct FlInternalState {
    HINSTANCE HInstance;
    HWND HWnd;
    VkSurfaceKHR Surface;
} FlInternalState;

// Clock
static FlFloat64 flClockFrequency;
static LARGE_INTEGER flStartTime;

LRESULT CALLBACK flWin32ProcessMessage(HWND hWnd, FlUInt32 msg, WPARAM wParam, LPARAM lParam);

FlBool8 flPlatformStartup(
    FlPlatformState* platformState,
    const char* applicationName,
    const FlInt32 x,
    const FlInt32 y,
    const FlInt32 width,
    const FlInt32 height) {
    platformState->InternalState = malloc(sizeof(FlInternalState));
    FlInternalState* state = (FlInternalState*)platformState->InternalState;

    state->HInstance = GetModuleHandleA(0); // 0 = Get the handle to the application executing this code.

    // Setup and register window class.
    const HICON icon = LoadIcon(state->HInstance, IDI_APPLICATION);
    WNDCLASSA windowClass = {0};
    windowClass.style = CS_DBLCLKS; // Get double clicks
    windowClass.lpfnWndProc = flWin32ProcessMessage;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = state->HInstance;
    windowClass.hIcon = icon;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW); // Manage the cursor manually.
    windowClass.hbrBackground = NULL; // Transparent
    windowClass.lpszClassName = "flashlightWindowClass";

    if (!RegisterClassA(&windowClass)) {
        MessageBoxA(NULL, "Window registration failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    // Create window
    // The client is the actual part of the window the applications interacts with.
    // The window also counts the title bar for example.
    const FlUInt32 clientX = x;
    const FlUInt32 clientY = y;
    const FlUInt32 clientWidth = width;
    const FlUInt32 clientHeight = height;

    FlUInt32 windowX = clientX;
    FlUInt32 windowY = clientY;
    FlUInt32 windowWidth = clientWidth;
    FlUInt32 windowHeight = clientHeight;

    FlUInt32 windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    const FlUInt32 windowExStyle = WS_EX_APPWINDOW;

    windowStyle |= WS_MAXIMIZEBOX;
    windowStyle |= WS_MINIMIZEBOX;
    windowStyle |= WS_THICKFRAME;

    // Obtain the size of the border
    RECT borderRect = {0, 0, 0, 0};
    AdjustWindowRectEx(&borderRect, windowStyle, false, windowExStyle);

    // In this case, the border rectangle is negative.
    windowX += borderRect.left;
    windowY += borderRect.top;

    // Grow by the size of the OS border.
    windowWidth = borderRect.right - borderRect.left;
    windowHeight = borderRect.bottom - borderRect.top;

    const HWND handle = CreateWindowExA(
        windowExStyle, windowClass.lpszClassName, applicationName,
        windowStyle, (FlInt32)windowX, (FlInt32)windowY, (FlInt32)clientWidth, (FlInt32)clientHeight,
        0, 0, state->HInstance, 0);

    if (handle == NULL) {
        MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        FL_LOG_FATAL("Window creation failed!")
        return false;
    }

    state->HWnd = handle;

    // Show the window
    const FlBool32 shouldActivate = true; // TODO: If the window should not accept input, this should be false.
    const FlInt32 showWindowCommandFlags = shouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;
    // If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVATE;
    // If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE
    ShowWindow(state->HWnd, showWindowCommandFlags);

    // Clock setup
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    flClockFrequency = 1.0 / (FlFloat64)frequency.QuadPart;
    QueryPerformanceCounter(&flStartTime);

    return true;
}

void flPlatformShutdown(FlPlatformState* platformState) {
    // Simply cold-cast to the known type.
    FlInternalState* state = (FlInternalState*)platformState->InternalState;

    if (state->HWnd) {
        DestroyWindow(state->HWnd);
        state->HWnd = NULL;
    }
}

FlBool8 flPlatformPumpMessages(FlPlatformState* platformState) {
    FL_UNUSED(platformState);

    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }

    return true;
}

// TODO: Temporary
void* flPlatformAllocate(const FlUInt64 size, const FlBool8 aligned) {
    FL_UNUSED(aligned);

    return malloc(size);
}

void flPlatformFree(void* block, const FlBool8 aligned) {
    FL_UNUSED(aligned);

    free(block);
}

void* flPlatformZeroMemory(void* dest, const FlUInt64 size) {
    return memset(dest, 0, size);
}

void* flPlatformCopyMemory(void* dest, const void* source, const FlUInt64 size) {
    return memcpy(dest, source, size);
}

void* flPlatformSetMemory(void* dest, const FlInt32 value, const FlUInt64 size) {
    return memset(dest, value, size);
}

void flPlatformConsoleWrite(const char* message, const FlUInt8 color) {
    const HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static FlUInt8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(consoleHandle, levels[color]);

    OutputDebugStringA(message);
    const FlUInt64 length = flStringLength(message);
    const LPDWORD numberWritten = 0;
    WriteConsoleA(consoleHandle, message, (DWORD)length, numberWritten, NULL);
}

void flPlatformConsoleWriteError(const char* message, FlUInt8 color) {
    const HANDLE consoleHandle = GetStdHandle(STD_ERROR_HANDLE);
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static FlUInt8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(consoleHandle, levels[color]);

    OutputDebugStringA(message);
    const FlUInt64 length = flStringLength(message);
    const LPDWORD numberWritten = 0;
    WriteConsoleA(consoleHandle, message, (DWORD)length, numberWritten, NULL);
}

FlFloat64 flPlatformGetAbsoluteTime(void) {
    LARGE_INTEGER nowTime;
    QueryPerformanceCounter(&nowTime);
    return (FlFloat64)nowTime.QuadPart * flClockFrequency;
}

void flPlatformSleep(const FlUInt64 milliseconds) {
    Sleep((DWORD)milliseconds);
}

void flPlatformGetRequiredExtensionNames(const char*** namesDArray) {
    flDArrayPush(*namesDArray, &"VK_KHR_win32_surface");
}

FlBool8 flPlatformCreateVulkanSurface(struct FlPlatformState* platformState,
                                      struct FlVulkanContext* context) {
    // Simply cold-cast to the known type.
    FlInternalState* state = (FlInternalState*)platformState->InternalState;

    VkWin32SurfaceCreateInfoKHR createInfo = {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    createInfo.hinstance = state->HInstance;
    createInfo.hwnd = state->HWnd;

    VkResult result = vkCreateWin32SurfaceKHR(
        context->Instance, 
        &createInfo, 
        context->Allocator, 
        &state->Surface);
    if (result != VK_SUCCESS) {
        FL_LOG_FATAL("Vulkan surface creation failed.");
        return false;
    }

    context->Surface = state->Surface;
    return true;
}

LRESULT CALLBACK flWin32ProcessMessage(const HWND hWnd, const FlUInt32 msg, const WPARAM wParam, const LPARAM lParam) {
    switch (msg) {
    case WM_ERASEBKGND:
        // Notify the OS that erasing will be handled by the application to prevent flickering.
        return 1;
    case WM_CLOSE:
        FlEventContext data = {};
        flEventFire(FlEventCodeApplicationQuit, 0, data);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_SIZE:
        {
            // Get the updated size
            RECT r;
            GetClientRect(hWnd, &r);
            FlUInt32 width = r.right - r.left;
            FlUInt32 height = r.bottom - r.top;

            FlEventContext context;
            context.data.uint16[0] = (FlUInt16)width;
            context.data.uint16[1] = (FlUInt16)height;
            flEventFire(FlEventCodeResized, 0, context);
            break;
        }
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        {
            // Key pressed/released
            const FlBool8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            FlKeys key = (FlUInt16)wParam;

            // Pass to the input subsystem for processing.
            flInputProcessKey(key, pressed);

            break;
        }
    case WM_MOUSEMOVE:
        {
            // Mouse move.
            FlInt32 xPosition = GET_X_LPARAM(lParam);
            FlInt32 yPosition = GET_Y_LPARAM(lParam);

            // Pass to the input subsystem for processing.
            flInputProcessMouseMove((FlInt16)xPosition, (FlInt16)yPosition);

            break;
        }
    case WM_MOUSEWHEEL:
        {
            FlInt32 zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (zDelta != 0) {
                // Flatten the input to an OS-independent (-1, 1)
                zDelta = (zDelta < 0) ? -1 : 1;

                // Pass to the input subsystem for processing.
                flInputProcessMouseWheel((FlInt8)zDelta); // zDelta was clamped to -1;1, so it's fine to convert it to a FlInt8 
            }

            break;
        }
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        {
            const FlBool8 pressed = (msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN);
            FlButtons mouseButton = FlButtonMaxButtons;
            
            switch (msg) {       
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
                mouseButton = FlButtonLeft;
                break;

            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
                mouseButton = FlButtonMiddle;
                break;

            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
                mouseButton = FlButtonRight;
                break;
            }

            // Pass to the input subsystem for processing.
            if (mouseButton != FlButtonMaxButtons) {
                flInputProcessButton(mouseButton, pressed);
            }

            break;
        }
    }

    return DefWindowProcA(hWnd, msg, wParam, lParam);
}

#endif
