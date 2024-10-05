// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#include "FlashlightEngine/Containers/DArray.h"

#include "FlashlightEngine/Core/Logger.h"
#include "FlashlightEngine/Core/FlMemory.h"

void* _flDArrayCreate(FlUInt64 length, FlUInt64 stride) {
    FlUInt64 headerSize = FlDarrayFieldLength * sizeof(FlUInt64);
    FlUInt64 arraySize = length * stride;
    FlUInt64* newArray = (FlUInt64*)flAllocate(headerSize + arraySize, FlMemoryTagDArray);
    flSetMemory(newArray, 0, headerSize + arraySize);
    newArray[FlDArrayCapacity] = length;
    newArray[FlDArrayLength] = 0;
    newArray[FlDArrayStride] = stride;

    return (void*)(newArray + FlDarrayFieldLength);
}

void _flDArrayDestroy(void* array) {
    FlUInt64* header = (FlUInt64*)array - FlDarrayFieldLength;
    FlUInt64 headerSize = FlDarrayFieldLength * sizeof(FlUInt64);
    FlUInt64 totalSize = headerSize + header[FlDArrayCapacity] * header[FlDArrayStride];
    flFree(header, totalSize, FlMemoryTagDArray);
}

FlUInt64 _flDArrayFieldGet(void* array, FlUInt64 field) {
    FlUInt64* header = (FlUInt64*)array - FlDarrayFieldLength;
    return header[field];
}

void _flDArrayFieldSet(void* array, FlUInt64 field, FlUInt64 value) {
    FlUInt64* header = (FlUInt64*)array - FlDarrayFieldLength;
    header[field] = value;
}

void* _flDArrayResize(void* array) {
    FlUInt64 length = flDArrayLength(array);
    FlUInt64 stride = flDArrayStride(array);
    void* temp = _flDArrayCreate(
        (FL_DARRAY_RESIZE_FACTOR * flDArrayCapacity(array)),
        stride);
    flCopyMemory(temp, array, length * stride);

    _flDArrayFieldSet(temp, FlDArrayLength, length);
    _flDArrayDestroy(array);

    return temp;
}

void* _flDArrayPush(void* array, const void* valuePtr) {
    FlUInt64 length = flDArrayLength(array);
    FlUInt64 stride = flDArrayStride(array);

    if (length >= flDArrayCapacity(array)) {
        array = _flDArrayResize(array);
    }

    FlUInt64 addr = (FlUInt64)array;
    addr += (length * stride);
    flCopyMemory((void*)addr, valuePtr, stride);
    _flDArrayFieldSet(array, FlDArrayLength, length + 1);
    
    return array;
}

void _flDArrayPop(void* array, void* dest) {
    FlUInt64 length = flDArrayLength(array);
    FlUInt64 stride = flDArrayStride(array);

    FlUInt64 addr = (FlUInt64)array;
    addr += ((length - 1) * stride);
    flCopyMemory(dest, (void*)addr, stride);
    _flDArrayFieldSet(array, FlDArrayLength, length - 1);
}

void* _flDArrayInsertAt(void* array, FlUInt64 index, void* valuePtr) {
    FlUInt64 length = flDArrayLength(array);
    FlUInt64 stride = flDArrayStride(array);

    if (index >= length) {
        FL_LOG_ERROR("Index outside of the bounds of this array! Length: %i, index %i", length, index);
        return array;
    }

    if (length >= flDArrayCapacity(array)) {
        array = _flDArrayResize(array);
    }

    FlUInt64 addr = (FlUInt64)array;
    
    // If not on the last element, copy the rest outward.
    if (index != length - 1) {
        flCopyMemory(
            (void*)(index + (index + 1) * stride),
            (void*)(addr + (index * stride)),
            stride * (length - index)
        );
    }

    // Set the value at the index.
    flCopyMemory((void*)(addr + (index * stride)), valuePtr, stride);

    _flDArrayFieldSet(array, FlDArrayLength, length + 1);
    return array;
}

void* _flDArrayPopAt(void* array, FlUInt64 index, void* dest) {
    FlUInt64 length = flDArrayLength(array);
    FlUInt64 stride = flDArrayStride(array);

    if (index >= length) {
        FL_LOG_ERROR("Index outside of the bounds of this array! Length: %i, index %i", length, index);
        return array;
    }

    FlUInt64 addr = (FlUInt64)array;
    flCopyMemory(dest, (void*)(addr + (index * stride)), stride);

    // If not on the last element, snip out the entry and copy the rest inward.
    if (index != length - 1) {
        flCopyMemory(
            (void*)(addr + (index * stride)),
            (void*)(index + (index + 1) * stride),
            stride * (length - index)
        );
    }

    _flDArrayFieldSet(array, FlDArrayLength, length - 1);
    return array;
}
