// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_CORE_FLMEMORY_H
#define FL_CORE_FLMEMORY_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

typedef enum FlMemoryTag {
    // For temporary use. Should be assigned one of the below or have a new tag created.
    FlMemoryTagUnknown,
    FlMemoryTagArray,
    FlMemoryTagDArray,
    FlMemoryTagDict,
    FlMemoryTagRingQueue,
    FlMemoryTagBST,
    FlMemoryTagString,
    FlMemoryTagApplication,
    FlMemoryTagJob,
    FlMemoryTagTexture,
    FlMemoryTagMaterialInstance,
    FlMemoryTagRenderer,
    FlMemoryTagGame,
    FlMemoryTagTransform,
    FlMemoryTagEntity,
    FlMemoryTagEntityNode,
    FlMemoryTagScene,

    FlMemoryTagMaxTags
} FlMemoryTag;

FL_API void flInitializeMemory(void);
FL_API void flShutdownMemory(void);

FL_API void* flAllocate(FlUInt64 size, FlMemoryTag memoryTag);

FL_API void flFree(void* block, FlUInt64 size, FlMemoryTag memoryTag);

FL_API void flZeroMemory(void* block, FlUInt64 size);

FL_API void flCopyMemory(void* dest, const void* source, FlUInt64 size);

FL_API void flSetMemory(void* dest, FlInt32 value, FlUInt64 size);

FL_API char* flGetMemoryUsageString(void);

#endif // FL_CORE_FLMEMORY_H
