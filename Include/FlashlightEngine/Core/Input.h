// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_CORE_INPUT_H
#define FL_CORE_INPUT_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

typedef enum FlButtons {
    FlButtonLeft,
    FlButtonRight,
    FlButtonMiddle,

    FlButtonMaxButtons
} FlButtons;

#define FL_DEFINE_KEY(name, code) FlKey##name = code

typedef enum FlKeys {
    FL_DEFINE_KEY(Backspace, 0x08),
    FL_DEFINE_KEY(Enter,     0x0D),
    FL_DEFINE_KEY(Tab,       0x09),
    FL_DEFINE_KEY(Shift,     0x10),
    FL_DEFINE_KEY(Control,   0x11),    
    
    FL_DEFINE_KEY(PAUSE,   0x13),
    FL_DEFINE_KEY(CAPITAL, 0x14),
    
    FL_DEFINE_KEY(Escape, 0x1B),
    
    FL_DEFINE_KEY(Convert,    0x1C),
    FL_DEFINE_KEY(NonConvert, 0x1D),
    FL_DEFINE_KEY(Accept,     0x1E),
    FL_DEFINE_KEY(ModeChange, 0x1F),
    
    FL_DEFINE_KEY(Space,    0x20),
    FL_DEFINE_KEY(Prior,    0x21),
    FL_DEFINE_KEY(Next,     0x22),
    FL_DEFINE_KEY(End,      0x23),
    FL_DEFINE_KEY(Home,     0x24),
    FL_DEFINE_KEY(Left,     0x25),
    FL_DEFINE_KEY(Up,       0x26),
    FL_DEFINE_KEY(Right,    0x27),
    FL_DEFINE_KEY(Down,     0x28),
    FL_DEFINE_KEY(Select,   0x29),
    FL_DEFINE_KEY(Print,    0x2A),
    FL_DEFINE_KEY(Execute,  0x2B),
    FL_DEFINE_KEY(Snapshot, 0x2C),
    FL_DEFINE_KEY(Insert,   0x2D),
    FL_DEFINE_KEY(Delete,   0x2E),
    FL_DEFINE_KEY(Help,     0x2F),

    FL_DEFINE_KEY(A, 0x41),
    FL_DEFINE_KEY(B, 0x42),
    FL_DEFINE_KEY(C, 0x43),
    FL_DEFINE_KEY(D, 0x44),
    FL_DEFINE_KEY(E, 0x45),
    FL_DEFINE_KEY(F, 0x46),
    FL_DEFINE_KEY(G, 0x47),
    FL_DEFINE_KEY(H, 0x48),
    FL_DEFINE_KEY(I, 0x49),
    FL_DEFINE_KEY(J, 0x4A),
    FL_DEFINE_KEY(K, 0x4B),
    FL_DEFINE_KEY(L, 0x4C),
    FL_DEFINE_KEY(M, 0x4D),
    FL_DEFINE_KEY(N, 0x4E),
    FL_DEFINE_KEY(O, 0x4F),
    FL_DEFINE_KEY(P, 0x50),
    FL_DEFINE_KEY(Q, 0x51),
    FL_DEFINE_KEY(R, 0x52),
    FL_DEFINE_KEY(S, 0x53),
    FL_DEFINE_KEY(T, 0x54),
    FL_DEFINE_KEY(U, 0x55),
    FL_DEFINE_KEY(V, 0x56),
    FL_DEFINE_KEY(W, 0x57),
    FL_DEFINE_KEY(X, 0x58),
    FL_DEFINE_KEY(Y, 0x59),
    FL_DEFINE_KEY(Z, 0x5A),

    FL_DEFINE_KEY(LWin, 0x5B),
    FL_DEFINE_KEY(RWin, 0x5C),
    FL_DEFINE_KEY(Apps, 0x5D),

    FL_DEFINE_KEY(Sleep, 0x5F),

    FL_DEFINE_KEY(Numpad0,   0x60),
    FL_DEFINE_KEY(Numpad1,   0x61),
    FL_DEFINE_KEY(Numpad2,   0x62),
    FL_DEFINE_KEY(Numpad3,   0x63),
    FL_DEFINE_KEY(Numpad4,   0x64),
    FL_DEFINE_KEY(Numpad5,   0x65),
    FL_DEFINE_KEY(Numpad6,   0x66),
    FL_DEFINE_KEY(Numpad7,   0x67),
    FL_DEFINE_KEY(Numpad8,   0x68),
    FL_DEFINE_KEY(Numpad9,   0x69),
    FL_DEFINE_KEY(Multiply,  0x6A),
    FL_DEFINE_KEY(Add,       0x6B),
    FL_DEFINE_KEY(Separator, 0x6C),
    FL_DEFINE_KEY(Subtract,  0x6D),
    FL_DEFINE_KEY(Decimal,   0x6E),
    FL_DEFINE_KEY(Divide,    0x6F),

    FL_DEFINE_KEY(F1,  0x70),
    FL_DEFINE_KEY(F2,  0x71),
    FL_DEFINE_KEY(F3,  0x72),
    FL_DEFINE_KEY(F4,  0x73),
    FL_DEFINE_KEY(F5,  0x74),
    FL_DEFINE_KEY(F6,  0x75),
    FL_DEFINE_KEY(F7,  0x76),
    FL_DEFINE_KEY(F8,  0x77),
    FL_DEFINE_KEY(F9,  0x78),
    FL_DEFINE_KEY(F10, 0x79),
    FL_DEFINE_KEY(F11, 0x7A),
    FL_DEFINE_KEY(F12, 0x7B),
    FL_DEFINE_KEY(F13, 0x7C),
    FL_DEFINE_KEY(F14, 0x7D),
    FL_DEFINE_KEY(F15, 0x7E),
    FL_DEFINE_KEY(F16, 0x7F),
    FL_DEFINE_KEY(F17, 0x80),
    FL_DEFINE_KEY(F18, 0x81),
    FL_DEFINE_KEY(F19, 0x82),
    FL_DEFINE_KEY(F20, 0x83),
    FL_DEFINE_KEY(F21, 0x84),
    FL_DEFINE_KEY(F22, 0x85),
    FL_DEFINE_KEY(F23, 0x86),
    FL_DEFINE_KEY(F24, 0x87),

    FL_DEFINE_KEY(Numlock, 0x90),
    FL_DEFINE_KEY(Scroll,  0x91),
    
    FL_DEFINE_KEY(NumpadEqual, 0x92),
    
    FL_DEFINE_KEY(LShift,   0xA0),
    FL_DEFINE_KEY(RShift,   0xA1),
    FL_DEFINE_KEY(LControl, 0xA2),
    FL_DEFINE_KEY(RControl, 0xA3),
    FL_DEFINE_KEY(LMenu,    0xA4),
    FL_DEFINE_KEY(RMenu,    0xA5),
    
    FL_DEFINE_KEY(Semicolon, 0xBA),
    FL_DEFINE_KEY(Plus,      0xBB),
    FL_DEFINE_KEY(Comma,     0xBC),
    FL_DEFINE_KEY(Minus,     0xBD),
    FL_DEFINE_KEY(Period,    0xBE),
    FL_DEFINE_KEY(Slash,     0xBF),
    FL_DEFINE_KEY(Grave,     0xC0),

    FlKeysMaxKeys
} FlKeys;

void flInputInitialize(void);
void flInputShutdown(void);
void flInputUpdate(FlFloat64 deltaTime);

// Keyboard input
FL_API FlBool8 flInputIsKeyDown(FlKeys key);
FL_API FlBool8 flInputIsKeyUp(FlKeys key);
FL_API FlBool8 flInputWasKeyDown(FlKeys key);
FL_API FlBool8 flInputWasKeyUp(FlKeys key);

void flInputProcessKey(FlKeys key, FlBool8 pressed);

// Mouse input
FL_API FlBool8 flInputIsButtonDown(FlButtons button);
FL_API FlBool8 flInputIsButtonUp(FlButtons button);
FL_API FlBool8 flInputWasButtonDown(FlButtons button);
FL_API FlBool8 flInputWasButtonUp(FlButtons button);
FL_API void flInputGetMousePosition(FlInt32* x, FlInt32* y);
FL_API void flInputGetPreviousMousePosition(FlInt32* x, FlInt32* y);

void flInputProcessButton(FlButtons button, FlBool8 pressed);
void flInputProcessMouseMove(FlInt16 x, FlInt16 y);
void flInputProcessMouseWheel(FlInt8 zDelta);

#endif // FL_CORE_INPUT_H
