// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_CONTAINERS_DARRAY_H
#define FL_CONTAINERS_DARRAY_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

/*
 * Memory layout:
 * FlUInt64 capacity = number of elements that can be held
 * FlUInt64 length = number of elements currently contained
 * FlUInt64 stride = size of each element in bytes
 * void* elements
 */
enum {
    FlDArrayCapacity,
    FlDArrayLength,
    FlDArrayStride,
    FlDarrayFieldLength
};

FL_API void* _flDArrayCreate(FlUInt64 length, FlUInt64 stride);
FL_API void _flDArrayDestroy(void* array);

FL_API FlUInt64 _flDArrayFieldGet(void* array, FlUInt64 field);
FL_API void _flDArrayFieldSet(void* array, FlUInt64 field, FlUInt64 value);

FL_API void* _flDArrayResize(void* array);

FL_API void* _flDArrayPush(void* array, const void* valuePtr);
FL_API void _flDArrayPop(void* array, void* dest);

FL_API void* _flDArrayInsertAt(void* array, FlUInt64 index, void* valuePtr);
FL_API void* _flDArrayPopAt(void* array, FlUInt64 index, void* dest);

#define FL_DARRAY_DEFAULT_CAPACITY 1
#define FL_DARRAY_RESIZE_FACTOR 2

#define flDArrayCreate(type)                                  \
    _flDArrayCreate(FL_DARRAY_DEFAULT_CAPACITY, sizeof(type))

#define flDArrayReserve(type, capacity)     \
    _flDArrayCreate(capacity, sizeof(type))

#define flDArrayDestroy(array) \
    _flDArrayDestroy(array)

#define flDArrayPush(array, value)           \
    {                                        \
        FL_TYPEOF(value) temp = value;       \
        array = _flDArrayPush(array, &temp); \
    }

#define flDArrayPop(array, valuePtr) \
    _flDArrayPop(array, valuePtr)

#define flDArrayInsertAt(array, index, value)           \
    {                                                   \
        FL_TYPEOF(value) temp = value;                  \
        array = _flDArrayInsertAt(array, index, &temp); \
    }

#define flDArrayPopAt(array, index, valuePtr) \
    _flDArrayPopAt(array, index, valuePtr)

#define flDArrayClear(array)                    \
    _flDArrayFieldSet(array, FlDArrayLength, 0)

#define flDArrayCapacity(array)                \
    _flDArrayFieldGet(array, FlDArrayCapacity)

#define flDArrayLength(array)                \
    _flDArrayFieldGet(array, FlDArrayLength)

#define flDArrayStride(array)                \
    _flDArrayFieldGet(array, FlDArrayStride)

#define flDArrayLengthSet(array, value)             \
    _flDArrayFieldSet(array, FlDArrayLength, value)

#endif // FL_CONTAINERS_DARRAY_H
