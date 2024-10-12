// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include <FlashlightEngine/Platform/Platform.h>

#ifdef FL_PLATFORM_MACOS

#include "FlashlightEngine/Containers/DArray.h"

#include "FlashlightEngine/Core/Event.h"
#include "FlashlightEngine/Core/Input.h"
#include "FlashlightEngine/Core/FlString.h"
#include "FlashlightEngine/Core/Logger.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct FlInternalState {
    GLFWwindow* Window;
} FlInternalState;

static FlFloat64 StartTime = 0;

static void flPlatformConsoleWriteFile(FILE* file, const char* message, FlUInt8 color);

static void flPlatformErrorCallback(FlInt32 error, const char* description);
static void flPlatformKeyCallback(GLFWwindow* window, FlInt32 key, FlInt32 scancode, FlInt32 action, FlInt32 mods);
static void flPlatformMouseButtonCallback(GLFWwindow* window, FlInt32 button, FlInt32 action, FlInt32 mods);
static void flPlatformCursorPositionCallback(GLFWwindow* window, FlFloat64 xPos, FlFloat64 yPos);
static void flPlatformScrollCallback(GLFWwindow* window, FlFloat64 xOffset, FlFloat64 yOffset);
static void flPlatformFramebufferSizeCallback(GLFWwindow* window, FlInt32 width, FlInt32 height);

static FlKeys flTranslateKey(FlInt32 key);

FlBool8 flPlatformStartup(FlPlatformState* platformState,
                          const char* applicationName,
                          FlInt32 x,
                          FlInt32 y,
                          FlInt32 width,
                          FlInt32 height) {
    platformState->InternalState = malloc(sizeof(FlInternalState));
    FlInternalState* state = (FlInternalState*)platformState->InternalState;

    glfwSetErrorCallback(flPlatformErrorCallback);
    if (!glfwInit()) {
        FL_LOG_FATAL("Failed to initialize GLFW.")
        return false;
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Do not create an OpenGL context.

    state->Window = glfwCreateWindow(width, height, applicationName, 0, 0);
    if (!state->Window) {
        FL_LOG_FATAL("Failed to create the window.");
        glfwTerminate();
        return false;
    }

    glfwSetKeyCallback(state->Window, flPlatformKeyCallback);
    glfwSetMouseButtonCallback(state->Window, flPlatformMouseButtonCallback);
    glfwSetCursorPosCallback(state->Window, flPlatformCursorPositionCallback);
    glfwSetScrollCallback(state->Window, flPlatformScrollCallback);
    glfwSetFramebufferSizeCallback(state->Window, flPlatformFramebufferSizeCallback);

    glfwSetWindowPos(state->Window, x, y);
    glfwShowWindow(state->Window);
    StartTime = glfwGetTime();

    return true;
}

void flPlatformShutdown(FlPlatformState* platformState) {
    FlInternalState* state = (FlInternalState*)platformState->InternalState;

    glfwDestroyWindow(state->Window);
    state->Window = 0;

    glfwSetErrorCallback(0);
    glfwTerminate();
}

FlBool8 flPlatformPumpMessages(FlPlatformState* platformState) {
    FlInternalState* state = (FlInternalState*)platformState->InternalState;

    glfwPollEvents();
    FlBool8 shouldContinue = !glfwWindowShouldClose(state->Window);
    return shouldContinue;
}

void* flPlatformAllocate(FlUInt64 size, FlBool8 aligned) {
    return malloc(size);
}

void flPlatformFree(void* block, FlBool8 aligned) {
    free(block);
}

void* flPlatformZeroMemory(void* dest, FlUInt64 size) {
    return memset(dest, 0, size);
}

void* flPlatformCopyMemory(void* dest, const void* source, FlUInt64 size) {
    return memcpy(dest, source, size);
}

void* flPlatformSetMemory(void* dest, FlInt32 value, FlUInt64 size) {
    return memset(dest, value, size);
}

void flPlatformConsoleWrite(const char* message, FlUInt8 color) {
    flPlatformConsoleWriteFile(stdout, message, color);
}

void flPlatformConsoleWriteError(const char* message, FlUInt8 color) {
    flPlatformConsoleWriteFile(stderr, message, color);
}

FlFloat64 flPlatformGetAbsoluteTime(void) {
    FlFloat64 time = glfwGetTime();
    return time;
}

void flPlatformSleep(FlUInt64 milliseconds) {
    struct timespec ts = {0};
    ts.tv_sec = (long)((FlFloat64)milliseconds * 0.001);
    ts.tv_nsec = ((long)milliseconds % 1000) * 1000 * 1000;
    nanosleep(&ts, 0);
}

void flPlatformGetRequiredExtensionNames(const char*** namesDArray) {
    FlUInt32 count = 0;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);
    for (FlUInt32 i = 0; i < count; ++i) {
        if (flStringsEqual(extensions[i], "VK_KHR_surface")) {
            // VK_KHR_surface is already included, skip it.
            continue;
        }
        flDArrayPush(*namesDArray, extensions[i]);
    }
}

FlBool8 flPlatformCreateVulkanSurface(struct FlPlatformState* platformState,
                                      struct FlVulkanContext* context) {
    // Simply cold-cast to the known type.
    FlInternalState* state = (FlInternalState*)platformState->InternalState;

    VkResult result = glfwCreateWindowSurface(context->Instance, state->Window, context->Allocator, &context->Surface);
    if (result != VK_SUCCESS) {
        FL_LOG_FATAL("Vulkan surface creation failed.")
        return false;
    }

    return true;
}

static void flPlatformConsoleWriteFile(FILE* file, const char* message, FlUInt8 color) {
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    const char* colorStrings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    fprintf(file, "\033[%sm%s\033[0m", colorStrings[color], message);
}

static void flPlatformErrorCallback(FlInt32 error, const char* description) {
    flPlatformConsoleWriteError(description, 0);
}

static void flPlatformKeyCallback(GLFWwindow* window, FlInt32 key, FlInt32 scancode, FlInt32 action, FlInt32 mods) {
    FlKeys flKey = flTranslateKey(key);
    if (flKey != FlKeysMaxKeys) {
        FlBool8 pressed = action == GLFW_PRESS || action == GLFW_REPEAT;
        flInputProcessKey(flKey, pressed);
    }
}

static void flPlatformMouseButtonCallback(GLFWwindow* window, FlInt32 button, FlInt32 action, FlInt32 mods) {
    FlButtons flButton = FlButtonMaxButtons;

    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        flButton = FlButtonLeft;
        break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
        flButton = FlButtonMiddle;
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        flButton = FlButtonRight;
        break;
    default:
        flButton = FlButtonMaxButtons;
        break;
    }

    if (flButton != FlButtonMaxButtons) {
        FlBool8 pressed = action == GLFW_PRESS;
        flInputProcessButton(flButton, pressed);
    }
}

static void flPlatformCursorPositionCallback(GLFWwindow* window, FlFloat64 xPos, FlFloat64 yPos) {
    flInputProcessMouseMove((FlInt16)xPos, (FlInt16)yPos);
}

static void flPlatformScrollCallback(GLFWwindow* window, FlFloat64 xOffset, FlFloat64 yOffset) {
    FlInt8 zDelta = (FlInt8)yOffset;
    if (zDelta != 0) {
        zDelta = (zDelta > 0) ? -1 : 1;
    }
    flInputProcessMouseWheel(zDelta);
}

static void flPlatformFramebufferSizeCallback(GLFWwindow* window, FlInt32 width, FlInt32 height) {
    FlEventContext context = {.data.uint16[0] = (FlUInt16)width, .data.uint16[1] = (FlUInt16)height};
    flEventFire(FlEventCodeResized, 0, context);
}

static FlKeys flTranslateKey(FlInt32 key) {
    FlKeys flKey = FlKeysMaxKeys;

    switch (key) {
    case GLFW_KEY_SPACE:
        flKey = FlKeySpace;
        break;
    case GLFW_KEY_COMMA:
        flKey = FlKeyComma;
        break;
    case GLFW_KEY_MINUS:
        flKey = FlKeyMinus;
        break;
    case GLFW_KEY_PERIOD:
        flKey = FlKeyPeriod;
        break;
    case GLFW_KEY_SLASH:
        flKey = FlKeySlash;
        break;
    case GLFW_KEY_0:
        flKey = FlKeyNumpad0;
        break;
    case GLFW_KEY_1:
        flKey = FlKeyNumpad1;
        break;
    case GLFW_KEY_2:
        flKey = FlKeyNumpad2;
        break;
    case GLFW_KEY_3:
        flKey = FlKeyNumpad3;
        break;
    case GLFW_KEY_4:
        flKey = FlKeyNumpad4;
        break;
    case GLFW_KEY_5:
        flKey = FlKeyNumpad5;
        break;
    case GLFW_KEY_6:
        flKey = FlKeyNumpad6;
        break;
    case GLFW_KEY_7:
        flKey = FlKeyNumpad7;
        break;
    case GLFW_KEY_8:
        flKey = FlKeyNumpad8;
        break;
    case GLFW_KEY_9:
        flKey = FlKeyNumpad9;
        break;
    case GLFW_KEY_SEMICOLON:
        flKey = FlKeySemicolon;
        break;
    case GLFW_KEY_EQUAL:
        flKey = FlKeyPlus;
        break;
    case GLFW_KEY_A:
        flKey = FlKeyA;
        break;
    case GLFW_KEY_B:
        flKey = FlKeyB;
        break;
    case GLFW_KEY_C:
        flKey = FlKeyC;
        break;
    case GLFW_KEY_D:
        flKey = FlKeyD;
        break;
    case GLFW_KEY_E:
        flKey = FlKeyE;
        break;
    case GLFW_KEY_F:
        flKey = FlKeyF;
        break;
    case GLFW_KEY_G:
        flKey = FlKeyG;
        break;
    case GLFW_KEY_H:
        flKey = FlKeyH;
        break;
    case GLFW_KEY_I:
        flKey = FlKeyI;
        break;
    case GLFW_KEY_J:
        flKey = FlKeyJ;
        break;
    case GLFW_KEY_K:
        flKey = FlKeyK;
        break;
    case GLFW_KEY_L:
        flKey = FlKeyL;
        break;
    case GLFW_KEY_M:
        flKey = FlKeyM;
        break;
    case GLFW_KEY_N:
        flKey = FlKeyN;
        break;
    case GLFW_KEY_O:
        flKey = FlKeyO;
        break;
    case GLFW_KEY_P:
        flKey = FlKeyP;
        break;
    case GLFW_KEY_Q:
        flKey = FlKeyQ;
        break;
    case GLFW_KEY_R:
        flKey = FlKeyR;
        break;
    case GLFW_KEY_S:
        flKey = FlKeyS;
        break;
    case GLFW_KEY_T:
        flKey = FlKeyT;
        break;
    case GLFW_KEY_U:
        flKey = FlKeyU;
        break;
    case GLFW_KEY_V:
        flKey = FlKeyV;
        break;
    case GLFW_KEY_W:
        flKey = FlKeyW;
        break;
    case GLFW_KEY_X:
        flKey = FlKeyX;
        break;
    case GLFW_KEY_Y:
        flKey = FlKeyY;
        break;
    case GLFW_KEY_Z:
        flKey = FlKeyZ;
        break;
    case GLFW_KEY_GRAVE_ACCENT:
        flKey = FlKeyGrave;
        break;
    case GLFW_KEY_ESCAPE:
        flKey = FlKeyEscape;
        break;
    case GLFW_KEY_ENTER:
        flKey = FlKeyEnter;
        break;
    case GLFW_KEY_TAB:
        flKey = FlKeyTab;
        break;
    case GLFW_KEY_BACKSPACE:
        flKey = FlKeyBackspace;
        break;
    case GLFW_KEY_INSERT:
        flKey = FlKeyInsert;
        break;
    case GLFW_KEY_DELETE:
        flKey = FlKeyDelete;
        break;
    case GLFW_KEY_RIGHT:
        flKey = FlKeyRight;
        break;
    case GLFW_KEY_LEFT:
        flKey = FlKeyLeft;
        break;
    case GLFW_KEY_DOWN:
        flKey = FlKeyDown;
        break;
    case GLFW_KEY_UP:
        flKey = FlKeyUp;
        break;
    case GLFW_KEY_PAGE_UP:
        flKey = FlKeyPrint;
        break;
    case GLFW_KEY_PAGE_DOWN:
        flKey = FlKeyNext;
        break;
    case GLFW_KEY_HOME:
        flKey = FlKeyHome;
        break;
    case GLFW_KEY_END:
        flKey = FlKeyEnd;
        break;
    case GLFW_KEY_CAPS_LOCK:
        flKey = FlKeyCapital;
        break;
    case GLFW_KEY_SCROLL_LOCK:
        flKey = FlKeyScroll;
        break;
    case GLFW_KEY_NUM_LOCK:
        flKey = FlKeyNumlock;
        break;
    case GLFW_KEY_PRINT_SCREEN:
        flKey = FlKeySnapshot;
        break;
    case GLFW_KEY_PAUSE:
        flKey = FlKeyPause;
        break;
    case GLFW_KEY_F1:
        flKey = FlKeyF1;
        break;
    case GLFW_KEY_F2:
        flKey = FlKeyF2;
        break;
    case GLFW_KEY_F3:
        flKey = FlKeyF3;
        break;
    case GLFW_KEY_F4:
        flKey = FlKeyF4;
        break;
    case GLFW_KEY_F5:
        flKey = FlKeyF5;
        break;
    case GLFW_KEY_F6:
        flKey = FlKeyF6;
        break;
    case GLFW_KEY_F7:
        flKey = FlKeyF7;
        break;
    case GLFW_KEY_F8:
        flKey = FlKeyF8;
        break;
    case GLFW_KEY_F9:
        flKey = FlKeyF9;
        break;
    case GLFW_KEY_F10:
        flKey = FlKeyF10;
        break;
    case GLFW_KEY_F11:
        flKey = FlKeyF11;
        break;
    case GLFW_KEY_F12:
        flKey = FlKeyF12;
        break;
    case GLFW_KEY_F13:
        flKey = FlKeyF13;
        break;
    case GLFW_KEY_F14:
        flKey = FlKeyF14;
        break;
    case GLFW_KEY_F15:
        flKey = FlKeyF15;
        break;
    case GLFW_KEY_F16:
        flKey = FlKeyF16;
        break;
    case GLFW_KEY_F17:
        flKey = FlKeyF17;
        break;
    case GLFW_KEY_F18:
        flKey = FlKeyF18;
        break;
    case GLFW_KEY_F19:
        flKey = FlKeyF19;
        break;
    case GLFW_KEY_F20:
        flKey = FlKeyF20;
        break;
    case GLFW_KEY_F21:
        flKey = FlKeyF21;
        break;
    case GLFW_KEY_F22:
        flKey = FlKeyF22;
        break;
    case GLFW_KEY_F23:
        flKey = FlKeyF23;
        break;
    case GLFW_KEY_F24:
        flKey = FlKeyF24;
        break;
    case GLFW_KEY_KP_0:
        flKey = FlKeyNumpad0;
        break;
    case GLFW_KEY_KP_1:
        flKey = FlKeyNumpad1;
        break;
    case GLFW_KEY_KP_2:
        flKey = FlKeyNumpad2;
        break;
    case GLFW_KEY_KP_3:
        flKey = FlKeyNumpad3;
        break;
    case GLFW_KEY_KP_4:
        flKey = FlKeyNumpad4;
        break;
    case GLFW_KEY_KP_5:
        flKey = FlKeyNumpad5;
        break;
    case GLFW_KEY_KP_6:
        flKey = FlKeyNumpad6;
        break;
    case GLFW_KEY_KP_7:
        flKey = FlKeyNumpad7;
        break;
    case GLFW_KEY_KP_8:
        flKey = FlKeyNumpad8;
        break;
    case GLFW_KEY_KP_9:
        flKey = FlKeyNumpad9;
        break;
    case GLFW_KEY_KP_DECIMAL:
        flKey = FlKeyDecimal;
        break;
    case GLFW_KEY_KP_DIVIDE:
        flKey = FlKeyDivide;
        break;
    case GLFW_KEY_KP_MULTIPLY:
        flKey = FlKeyMultiply;
        break;
    case GLFW_KEY_KP_SUBTRACT:
        flKey = FlKeySubtract;
        break;
    case GLFW_KEY_KP_ADD:
        flKey = FlKeyAdd;
        break;
    case GLFW_KEY_KP_ENTER:
        flKey = FlKeyEnter;
        break;
    case GLFW_KEY_KP_EQUAL:
        flKey = FlKeyNumpadEqual;
        break;
    case GLFW_KEY_LEFT_SHIFT:
        flKey = FlKeyLShift;
        break;
    case GLFW_KEY_LEFT_CONTROL:
        flKey = FlKeyLControl;
        break;
    case GLFW_KEY_LEFT_ALT:
        flKey = FlKeyLAlt;
        break;
    case GLFW_KEY_LEFT_SUPER:
        flKey = FlKeyLWin;
        break;
    case GLFW_KEY_RIGHT_SHIFT:
        flKey = FlKeyRShift;
        break;
    case GLFW_KEY_RIGHT_CONTROL:
        flKey = FlKeyRControl;
        break;
    case GLFW_KEY_RIGHT_ALT:
        flKey = FlKeyRAlt;
        break;
    case GLFW_KEY_RIGHT_SUPER:
        flKey = FlKeyRWin;
        break;
    default:
        // GLFW_KEY_UNKNOWN
        // GLFW_KEY_LAST
        // GLFW_KEY_APOSTROPHE
        // GLFW_KEY_LEFT_BRACKET
        // GLFW_KEY_BACKSLASH
        // GLFW_KEY_RIGHT_BRACKET
        // GLFW_KEY_F25
        // GLFW_KEY_WORLD_1
        // GLFW_KEY_WORLD_2
        // GLFW_KEY_MENU
        flKey = FlKeysMaxKeys;
    }
    
    return flKey;
}

#endif // FL_PLATFORM_MACOS
