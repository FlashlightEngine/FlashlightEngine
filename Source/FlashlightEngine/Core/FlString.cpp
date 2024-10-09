// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Core/FlString.hpp"

#include "FlashlightEngine/Core/FlMemory.h"

#include <cstring>

namespace Fl {

UInt64 flStringLength(const char* str) {
    return strlen(str);
}

bool flStringsEqual(const char* lhs, const char* rhs) {
    return strcmp(lhs, rhs) == 0;
}

}