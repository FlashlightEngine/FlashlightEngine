// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Core/FlString.h"

#include "FlashlightEngine/Core/FlMemory.h"

#include <string.h>

FL_API char* flStringDuplicate(const char* str) {
    FlUInt64 length = strlen(str);
    char* copy = flAllocate(length + 1, FlMemoryTagString);
    flCopyMemory(copy, str, length + 1);
    
    return copy;
}
