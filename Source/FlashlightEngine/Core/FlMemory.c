// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Core/FlMemory.h"

#include "FlashlightEngine/Core/Logger.h"
#include "FlashlightEngine/Platform/Platform.h"

// TODO: Custom string lib
#include <string.h>
#include <stdio.h>

// MSVC tells us to not use strdup and use _strdup instead, but _strdup doesn't exist on other compilers.
#if _MSC_VER
#define strdup _strdup
#endif

struct FlMemoryStats {
    FlUInt64 TotalAllocated;
    FlUInt64 TaggedAllocations[FlMemoryTagMaxTags];
};

static struct FlMemoryStats stats;

static const char* memoryTagStrings[FlMemoryTagMaxTags] = {
    "UNKNOWN     ",
    "ARRAY       ",
    "DARRAY      ",
    "DICT        ",
    "RING_QUEUE  ",
    "BST         ",
    "STRING      ",
    "APPLICATION ",
    "JOB         ",
    "TEXTURE     ",
    "MAT_INST    ",
    "RENDERER    ",
    "GAME        ",
    "TRANSFORM   ",
    "ENTITY      ",
    "ENTITY_NODE ",
    "SCENE       "
};

static FlBool8 Initialized = FALSE;

void flInitializeMemory(void) {
    if (Initialized) {
        return;
    }

    flPlatformZeroMemory(&stats, sizeof(stats));
    Initialized = TRUE;
    FL_LOG_INFO("Memory subsystem initialized.")
}

void flShutdownMemory(void) {
    Initialized = FALSE;
}

void* flAllocate(FlUInt64 size, FlMemoryTag memoryTag) {
    if (memoryTag == FlMemoryTagUnknown) {
        FL_LOG_WARN("flAllocate called with memoryTag set to FlMemoryTagUnknown. Re-class this allocation.");
    }

    stats.TotalAllocated += size;
    stats.TaggedAllocations[memoryTag] += size;

    // TODO: Memory alignment
    void* block = flPlatformAllocate(size, FALSE);
    flPlatformZeroMemory(block, size);

    return block;
}

void flFree(void* block, FlUInt64 size, FlMemoryTag memoryTag) {
    if (memoryTag == FlMemoryTagUnknown) {
        FL_LOG_WARN("flFree called with memoryTag set to FlMemoryTagUnknown. Re-class this allocation.");
    }

    stats.TotalAllocated -= size;
    stats.TaggedAllocations[memoryTag] -= size;

    // TODO: Memory alignment
    flPlatformFree(block, FALSE);
}

void flZeroMemory(void* block, FlUInt64 size) {
    flPlatformZeroMemory(block, size);
}

void flCopyMemory(void* dest, const void* source, FlUInt64 size) {
    flPlatformCopyMemory(dest, source, size);
}

void flSetMemory(void* dest, FlInt32 value, FlUInt64 size) {
    flPlatformSetMemory(dest, value, size);
}

char* flGetMemoryUsageString(void) {
    const FlUInt64 gib = 1024 * 1024 * 1024;
    const FlUInt64 mib = 1024 * 1024;
    const FlUInt64 kib = 1024;

    char buffer[8000] = "System memory use (tagged):\n";
    FlUInt64 offset = strlen(buffer);

    for (FlUInt32 i = 0; i < FlMemoryTagMaxTags; ++i) {
        char unit[4] = "XiB";
        FlFloat32 amount = 1.0f;

        if (stats.TaggedAllocations[i] >= gib) {
            unit[0] = 'G';
            amount = stats.TaggedAllocations[i] / (FlFloat32)gib;
        } else if (stats.TaggedAllocations[i] >= mib) {
            unit[0] = 'M';
            amount = stats.TaggedAllocations[i] / (FlFloat32)mib;
        } else if (stats.TaggedAllocations[i] >= kib) {
            unit[0] = 'K';
            amount = stats.TaggedAllocations[i] / (FlFloat32)kib;
        } else {
            unit[0] = 'B';
            unit[1] = 0;
            amount = (FlFloat32)stats.TaggedAllocations[i];
        }

        FlInt32 length = snprintf(buffer + offset, 8000, "%s: %.2f%s\n", memoryTagStrings[i], amount, unit);
        offset += length;
    }

    char* outString = strdup(buffer);

    return outString;
}