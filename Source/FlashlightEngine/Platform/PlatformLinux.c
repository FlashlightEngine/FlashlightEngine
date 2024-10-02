// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Platform/Platform.h"

#ifdef FL_PLATFORM_LINUX

#include "FlashlightEngine/Core/Logger.h"
#include "FlashlightEngine/Core/Event.h"
#include "FlashlightEngine/Core/Input.h"
#include "FlashlightEngine/Core/FlString.h"

#include "FlashlightEngine/Containers/DArray.h"

#include "FlashlightEngine/Renderer/Vulkan/VulkanPlatform.h"

#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <sys/time.h>

#include <time.h>  // nanosleep

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// For surface creation.
#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.h>
#include "FlashlightEngine/Renderer/Vulkan/VulkanTypes.inl"

typedef struct FlInternalState {
    Display* Display;
    xcb_connection_t* Connection;
    xcb_window_t Window;
    xcb_screen_t* Screen;
    xcb_atom_t WmProtocols;
    xcb_atom_t WmDeleteWin;
    VkSurfaceKHR Surface;
} FlInternalState;

// Key translation
FlKeys flTranslateKeycode(FlUInt32 xKeycode);

FlBool8 flPlatformStartup(
    FlPlatformState* platformState,
    const char* applicationName,
    FlInt32 x,
    FlInt32 y,
    FlInt32 width,
    FlInt32 height) {
    // Create the internal state.
    platformState->InternalState = malloc(sizeof(FlInternalState));
    FlInternalState* state = (FlInternalState*)platformState->InternalState;

    // Connect to X
    state->Display = XOpenDisplay(NULL);

    // Turn off key repeats.
    XAutoRepeatOff(state->Display);

    // Retrieve the connection from the display.
    state->Connection = XGetXCBConnection(state->Display);

    if (xcb_connection_has_error(state->Connection)) {
        FL_LOG_FATAL("Failed to connect to X server via XCB")
        return FALSE;
    }

    // Get data from the X server
    const struct xcb_setup_t* setup = xcb_get_setup(state->Connection);

    // Loop through the screens using iterator
    xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
    FlInt32 screenP = 0;
    for (FlInt32 s = screenP; s > 0; s--) {
        xcb_screen_next(&it);
    }

    // After screens have been looped through, assign it.
    state->Screen = it.data;

    // Allocate a XID for the window to be created.
    state->Window = xcb_generate_id(state->Connection);

    // Register event types.
    // XCB_CW_BACK_PIXEL = filling the window then window bg with a single color.
    // XCB_CW_EVENT_MASK is required.
    FlUInt32 eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    // Listen for keyboard and mouse buttons.
    FlUInt32 eventValues = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                           XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                           XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                           XCB_EVENT_MASK_STRUCTURE_NOTIFY;
    
    // Values to be sent over XCB (bg color, events)
    FlUInt32 valueList[] = {state->Screen->black_pixel, eventValues};

    // Create the window
    xcb_void_cookie_t cookie = xcb_create_window(
        state->Connection,
        XCB_COPY_FROM_PARENT,          // Depth
        state->Window,
        state->Screen->root,           // Parent
        x,                             // X position
        y,                             // Y position
        width,                         // Width
        height,                        // Height
        0,                             // No border
        XCB_WINDOW_CLASS_INPUT_OUTPUT, // Class
        state->Screen->root_visual,
        eventMask,
        valueList
    );
    FL_UNUSED(cookie);

    // Set the window's title
    xcb_change_property(
        state->Connection,
        XCB_PROP_MODE_REPLACE,
        state->Window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8, // Data should be viewed 8 bits at a time
        flStringLength(applicationName),
        applicationName
    );

    // Tell the server to notify when the window manager
    // attempts to destroy the window.
    xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(
        state->Connection,
        0,
        flStringLength("WM_DELETE_WINDOW"),
        "WM_DELETE_WINDOW"
    );

    xcb_intern_atom_cookie_t wmProtocolsCookie = xcb_intern_atom(
        state->Connection,
        0,
        flStringLength("WM_PROTOCOLS"),
        "WM_PROTOCOLS"
    );

    xcb_intern_atom_reply_t* wmDeleteReply = xcb_intern_atom_reply(
        state->Connection,
        wmDeleteCookie,
        NULL
    );

    xcb_intern_atom_reply_t* wmProtocolsReply = xcb_intern_atom_reply(
        state->Connection,
        wmProtocolsCookie,
        NULL
    );
    state->WmDeleteWin = wmDeleteReply->atom;
    state->WmProtocols = wmProtocolsReply->atom;

    xcb_change_property(
        state->Connection,
        XCB_PROP_MODE_REPLACE,
        state->Window,
        wmProtocolsReply->atom,
        4,
        32,
        1,
        &wmDeleteReply->atom
    );

    // Map the window to the screen
    xcb_map_window(state->Connection, state->Window);

    FlInt32 streamResult = xcb_flush(state->Connection);
    if (streamResult <= 0) {
        FL_LOG_FATAL("An error occured when flushing the stream: %d", streamResult)
        return FALSE;
    }

    return TRUE;
}

void flPlatformShutdown(FlPlatformState* platformState) {
    // Simply cold-cast to the known type.
    FlInternalState* state = (FlInternalState*)platformState->InternalState;

    // Turn key repeats back on since this is global for the OS.
    XAutoRepeatOn(state->Display);

    xcb_destroy_window(state->Connection, state->Window);
}

FlBool8 flPlatformPumpMessages(FlPlatformState* platformState) {
    // Simply cold-cast to the known type.
    FlInternalState* state = (FlInternalState*)platformState->InternalState;

    xcb_generic_event_t* event;
    xcb_client_message_event_t* clientMessage;

    FlBool8 quitFlagged = FALSE;

    while (event != 0) {
        event = xcb_poll_for_event(state->Connection);
        if (event == 0) {
            break;
        }

        // Input events
        switch (event->response_type & ~0x80) {
        case XCB_KEY_PRESS:
        case XCB_KEY_RELEASE:
            {
                // Key press event - xcb_key_press_event_t and xcb_key_release_event_t are the same
                xcb_key_press_event_t* kbEvent = (xcb_key_press_event_t*)event;
                FlBool8 pressed = event->response_type == XCB_KEY_PRESS;

                xcb_keycode_t code = kbEvent->detail;
                KeySym keySym = XkbKeycodeToKeysym(
                    state->Display,
                    (KeyCode)code, // event.xkey.keycode
                    0,
                    code & ShiftMask ? 1 : 0
                );

                FlKeys key = flTranslateKeycode(keySym);

                // Pass to the input subsystem for processing.
                flInputProcessKey(key, pressed);

                break;
            }
        case XCB_BUTTON_PRESS:
        case XCB_BUTTON_RELEASE:
            {
                xcb_button_press_event_t* mouseEvent = (xcb_button_press_event_t*)event;
                FlBool8 pressed = event->response_type == XCB_BUTTON_PRESS;
                FlButtons mouseButton = FlButtonMaxButtons;

                switch(mouseEvent->detail) {
                case XCB_BUTTON_INDEX_1:
                    mouseButton = FlButtonLeft;
                    break;
                case XCB_BUTTON_INDEX_2:
                    mouseButton = FlButtonMiddle;
                    break;
                case XCB_BUTTON_INDEX_3:
                    mouseButton = FlButtonRight;
                    break;
                }

                // Pass to the input subsystem for processing.
                if (mouseButton != FlButtonMaxButtons) {
                    flInputProcessButton(mouseButton, pressed);
                }

                break;
            }
        case XCB_MOTION_NOTIFY:
            {
                // Mouse move
                xcb_motion_notify_event_t* moveEvent = (xcb_motion_notify_event_t*)event;

                // Pass to the input subsystem for processing.
                flInputProcessMouseMove(moveEvent->event_x, moveEvent->event_y);
                break;
            }
        case XCB_CONFIGURE_NOTIFY:
            {
                // TODO: Resizing.
                break;
            }
        case XCB_CLIENT_MESSAGE:
            {
                clientMessage = (xcb_client_message_event_t*)event;

                // Window close
                if (clientMessage->data.data32[0] == state->WmDeleteWin) {
                    quitFlagged = TRUE;
                }
                break;
            }
        default:
            break;
        }

        free(event);
    }

    return !quitFlagged;
}

void* flPlatformAllocate(FlUInt64 size, FlBool8 aligned) {
    FL_UNUSED(aligned);
    return malloc(size);
}

void flPlatformFree(void* block, FlBool8 aligned) {
    FL_UNUSED(aligned);
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
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    const char* colorStrings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colorStrings[color], message);
}

void flPlatformConsoleWriteError(const char* message, FlUInt8 color) {
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    const char* colorStrings[] = {"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m", colorStrings[color], message);
}

FlFloat64 flPlatformGetAbsoluteTime(void) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec * 0.000000001;
}

void flPlatformSleep(FlUInt64 milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000 * 1000;
    nanosleep(&ts, 0);
}

void flPlatformGetRequiredExtensionNames(const char*** namesDArray) {
    flDArrayPush(*namesDArray, &"VK_KHR_xcb_surface");
}

FlBool8 flPlatformCreateVulkanSurface(struct FlPlatformState* platformState,
                                      struct FlVulkanContext* context) {
    // Simply cold-cast to the known type.
    FlInternalState* state = (FlInternalState*)platformState->InternalState;

    VkXcbSurfaceCreateInfoKHR createInfo = {VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR};
    createInfo.connection = state->Connection;
    createInfo.window = state->Window;

    VkResult result = vkCreateXcbSurfaceKHR(
        context->Instance, 
        &createInfo, 
        context->Allocator, 
        &state->Surface);
    if (result != VK_SUCCESS) {
        FL_LOG_FATAL("Vulkan surface creation failed.")
        return FALSE;
    }

    context->Surface = state->Surface;
    return TRUE;
}

FlKeys flTranslateKeycode(FlUInt32 xKeycode) {
    switch (xKeycode) {
    case XK_BackSpace:
        return FlKeyBackspace;
    case XK_Return:
        return FlKeyEnter;
    case XK_Tab:
        return FlKeyTab;
        // Not supported
        // case XK_Shift: return FlKeyShift;
        //case XK_Control: return FlKeyControl;
    case XK_Pause:
        return FlKeyPause;
    case XK_Caps_Lock:
        return FlKeyCapital;
    case XK_Escape:
        return FlKeyEscape;
        // Not supported
        // case : return FlKeyConvert;
        // case : return FlKeyNonConvert;
        // case : return FlKeyAccept;
    case XK_Mode_switch:
        return FlKeyModeChange;
    case XK_space:
        return FlKeySpace;
    case XK_Prior:
        return FlKeyPrior;
    case XK_Next:
        return FlKeyNext;
    case XK_End:
        return FlKeyEnd;
    case XK_Home:
        return FlKeyHome;
    case XK_Left:
        return FlKeyLeft;
    case XK_Up:
        return FlKeyUp;
    case XK_Right:
        return FlKeyRight;
    case XK_Down:
        return FlKeyDown;
    case XK_Select:
        return FlKeySelect;
    case XK_Print:
        return FlKeyPrint;
    case XK_Execute:
        return FlKeyExecute;
    // case XK_snapshot: return FlKeySnapshot; // not supported
    case XK_Insert:
        return FlKeyInsert;
    case XK_Delete:
        return FlKeyDelete;
    case XK_Help:
        return FlKeyHelp;
    case XK_Meta_L:
        return FlKeyLWin;  // TODO: not sure this is right
    case XK_Meta_R:
        return FlKeyRWin;
        // case XK_apps: return FlKeyApps; // not supported
        // case XK_sleep: return FlKeySleep; //not supported
    case XK_KP_0:
        return FlKeyNumpad0;
    case XK_KP_1:
        return FlKeyNumpad1;
    case XK_KP_2:
        return FlKeyNumpad2;
    case XK_KP_3:
        return FlKeyNumpad3;
    case XK_KP_4:
        return FlKeyNumpad4;
    case XK_KP_5:
        return FlKeyNumpad5;
    case XK_KP_6:
        return FlKeyNumpad6;
    case XK_KP_7:
        return FlKeyNumpad7;
    case XK_KP_8:
        return FlKeyNumpad8;
    case XK_KP_9:
        return FlKeyNumpad9;
    case XK_multiply:
        return FlKeyMultiply;
    case XK_KP_Add:
        return FlKeyAdd;
    case XK_KP_Separator:
        return FlKeySeparator;
    case XK_KP_Subtract:
        return FlKeySubtract;
    case XK_KP_Decimal:
        return FlKeyDecimal;
    case XK_KP_Divide:
        return FlKeyDivide;
    case XK_F1:
        return FlKeyF1;
    case XK_F2:
        return FlKeyF2;
    case XK_F3:
        return FlKeyF3;
    case XK_F4:
        return FlKeyF4;
    case XK_F5:
        return FlKeyF5;
    case XK_F6:
        return FlKeyF6;
    case XK_F7:
        return FlKeyF7;
    case XK_F8:
        return FlKeyF8;
    case XK_F9:
        return FlKeyF9;
    case XK_F10:
        return FlKeyF10;
    case XK_F11:
        return FlKeyF11;
    case XK_F12:
        return FlKeyF12;
    case XK_F13:
        return FlKeyF13;
    case XK_F14:
        return FlKeyF14;
    case XK_F15:
        return FlKeyF15;
    case XK_F16:
        return FlKeyF16;
    case XK_F17:
        return FlKeyF17;
    case XK_F18:
        return FlKeyF18;
    case XK_F19:
        return FlKeyF19;
    case XK_F20:
        return FlKeyF20;
    case XK_F21:
        return FlKeyF21;
    case XK_F22:
        return FlKeyF22;
    case XK_F23:
        return FlKeyF23;
    case XK_F24:
        return FlKeyF24;
    case XK_Num_Lock:
        return FlKeyNumlock;
    case XK_Scroll_Lock:
        return FlKeyScroll;
    case XK_KP_Equal:
        return FlKeyNumpadEqual;
    case XK_Shift_L:
        return FlKeyLShift;
    case XK_Shift_R:
        return FlKeyRShift;
    case XK_Control_L:
        return FlKeyLControl;
    case XK_Control_R:
        return FlKeyRControl;
    // case XK_Menu: return FlKeyLMenu;
    case XK_Menu:
        return FlKeyRMenu;
    case XK_semicolon:
        return FlKeySemicolon;
    case XK_plus:
        return FlKeyPlus;
    case XK_comma:
        return FlKeyComma;
    case XK_minus:
        return FlKeyMinus;
    case XK_period:
        return FlKeyPeriod;
    case XK_slash:
        return FlKeySlash;
    case XK_grave:
        return FlKeyGrave;
    case XK_a:
    case XK_A:
        return FlKeyA;
    case XK_b:
    case XK_B:
        return FlKeyB;
    case XK_c:
    case XK_C:
        return FlKeyC;
    case XK_d:
    case XK_D:
        return FlKeyD;
    case XK_e:
    case XK_E:
        return FlKeyE;
    case XK_f:
    case XK_F:
        return FlKeyF;
    case XK_g:
    case XK_G:
        return FlKeyG;
    case XK_h:
    case XK_H:
        return FlKeyH;
    case XK_i:
    case XK_I:
        return FlKeyI;
    case XK_j:
    case XK_J:
        return FlKeyJ;
    case XK_k:
    case XK_K:
        return FlKeyK;
    case XK_l:
    case XK_L:
        return FlKeyL;
    case XK_m:
    case XK_M:
        return FlKeyM;
    case XK_n:
    case XK_N:
        return FlKeyN;
    case XK_o:
    case XK_O:
        return FlKeyO;
    case XK_p:
    case XK_P:
        return FlKeyP;
    case XK_q:
    case XK_Q:
        return FlKeyQ;
    case XK_r:
    case XK_R:
        return FlKeyR;
    case XK_s:
    case XK_S:
        return FlKeyS;
    case XK_t:
    case XK_T:
        return FlKeyT;
    case XK_u:
    case XK_U:
        return FlKeyU;
    case XK_v:
    case XK_V:
        return FlKeyV;
    case XK_w:
    case XK_W:
        return FlKeyW;
    case XK_x:
    case XK_X:
        return FlKeyX;
    case XK_y:
    case XK_Y:
        return FlKeyY;
    case XK_z:
    case XK_Z:
        return FlKeyZ;
    default:
        return 0;
    }
}

#endif // FL_PLATFORM_LINUX