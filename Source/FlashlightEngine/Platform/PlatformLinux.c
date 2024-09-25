// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Platform/Platform.h"

#ifdef FL_PLATFORM_LINUX

#include "FlashlightEngine/Core/Logger.h"

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

typedef struct FlInternalState {
    Display* Display;
    xcb_connection_t* Connection;
    xcb_window_t Window;
    xcb_screen_t* Screen;
    xcb_atom_t WmProtocols;
    xcb_atom_t WmDeleteWin;
} FlInternalState;

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
        strlen(applicationName),
        applicationName
    );

    // Tell the server to notify when the window manager
    // attempts to destroy the window.
    xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(
        state->Connection,
        0,
        strlen("WM_DELETE_WINDOW"),
        "WM_DELETE_WINDOW"
    );

    xcb_intern_atom_cookie_t wmProtocolsCookie = xcb_intern_atom(
        state->Connection,
        0,
        strlen("WM_PROTOCOLS"),
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
                // TODO: Key presses and releases
                break;
            }
        case XCB_BUTTON_PRESS:
        case XCB_BUTTON_RELEASE:
            {
                // TODO: Mouse button presses and releases
                break;
            }
        case XCB_MOTION_NOTIFY:
            {
                // TODO: Mouse movement.
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

#endif // FL_PLATFORM_LINUX