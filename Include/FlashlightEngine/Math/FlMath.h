// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_MATH_FLMATH_H
#define FL_MATH_FLMATH_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

#include "FlashlightEngine/Math/MathTypes.h"

#include "FlashlightEngine/Core/FlMemory.h"

#define FL_PI 3.14159265358979323846f
#define FL_TWO_PI 2.0f * FL_PI
#define FL_HALF_PI 0.5f * FL_PI
#define FL_QUARTER_PI 0.25f * FL_PI
#define FL_ONE_OVER_PI 1.0f / FL_PI
#define FL_ONE_OVER_TWO_PI 1.0f / FL_TWO_PI
#define FL_SQRT_TWO 1.41421356237309504880f
#define FL_SQRT_THREE 1.73205080756887729352f
#define FL_SQRT_ONE_OVER_TWO 0.70710678118654752440f
#define FL_SQRT_ONE_OVER_THREE 0.57735026918962576450f
#define FL_DEG_TO_RAD_MULTIPLIER FL_PI / 180.0f
#define FL_RAD_TO_DEG_MULTIPLIER  180.0f / FL_PI

#define FL_SEC_TO_MS_MULTIPLIER 1000.0f

#define FL_MS_TO_SEC_MULTIPLIER 0.001f

// A huge number that should be larger than any valid number used.
#define FL_INFINITY 1e30f

// Smallest positive number where 1.0 + FL_FLOAT_EPSILON != 0
#define FL_FLOAT_EPSILON 1.192092896e-07f

// ---------------------- //
// General math functions //
// ---------------------- //

FL_API FlFloat32 flSin(FlFloat32 x);
FL_API FlFloat32 flCos(FlFloat32 x);
FL_API FlFloat32 flTan(FlFloat32 x);
FL_API FlFloat32 flACos(FlFloat32 x);
FL_API FlFloat32 flSqrt(FlFloat32 x);
FL_API FlFloat32 flAbs(FlFloat32 x);

/**
 * Indicates if the value is a power of two. Zero is considered not a power of two.
 * @param value The value to be checked.
 * @returns True if the value is a power of two, otherwise false.
 */
FL_INLINE FlBool8 flIsPowerOfTwo(FlUInt64 value) {
    return (value != 0) && ((value & (value - 1)) == 0);
}

FL_API FlInt32 flRandom();
FL_API FlInt32 flRandomInRange(FlInt32 min, FlInt32 max);

FL_API FlFloat32 flFRandom();
FL_API FlFloat32 flFRandomInRange(FlFloat32 min, FlFloat32 max);

// -------- //
// Vector 2 //
// -------- //

/**
 * @brief Creates and returns a new 2 element vector using the supplied values.
 * 
 * @param x The x value.
 * @param y The y value.
 * @return A new 2 element vector.
 */
FL_INLINE FlVec2 flVec2Create(FlFloat32 x, FlFloat32 y) {
    return (FlVec2){x, y};
}

/**
 * @brief Creates and returns a new 2 element vector with all components set to 0.0f.
 */
FL_INLINE FlVec2 flVec2Zero() {
    return (FlVec2){0.0f, 0.0f};
}

/**
 * @brief Creates and returns a new 2 element vector with all components set to 1.0f.
 */
FL_INLINE FlVec2 flVec2One() {
    return (FlVec2){1.0f, 1.0f};
}

/**
 * @brief Creates and returns a 2 element vector pointing up (0, 1).
 */
FL_INLINE FlVec2 flVec2Up() {
    return (FlVec2){0.0f, 1.0f};
}

/**
 * @brief Creates and returns a 2 element vector pointing down (0, -1).
 */
FL_INLINE FlVec2 flVec2Down() {
    return (FlVec2){0.0f, -1.0f};
}

/**
 * @brief Creates and returns a 2 element vector pointing left (-1, 0).
 */
FL_INLINE FlVec2 flVec2Left() {
    return (FlVec2){-1.0f, 0.0f};
}

/**
 * @brief Creates and returns a 2 element vector pointing right (1, 0).
 */
FL_INLINE FlVec2 flVec2Right() {
    return (FlVec2){1.0f, 0.0f};
}

/**
 * @brief Adds vector1 to vector0 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The resulting vector.
 */
FL_INLINE FlVec2 flVec2Add(FlVec2 vector0, FlVec2 vector1) {
    return (FlVec2){
        vector0.X + vector1.X,
        vector0.Y + vector1.Y
    };
}

/**
 * @brief Subtracts vector1 from vector0 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The resulting vector.
 */
FL_INLINE FlVec2 flVec2Sub(FlVec2 vector0, FlVec2 vector1) {
    return (FlVec2){
        vector0.X - vector1.X,
        vector0.Y - vector1.Y
    };
}

/**
 * @brief Multiplies vector0 with vector1 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The resulting vector.
 */
FL_INLINE FlVec2 flVec2Mul(FlVec2 vector0, FlVec2 vector1) {
    return (FlVec2){
        vector0.X * vector1.X,
        vector0.Y * vector1.Y
    };
}

/**
 * @brief Divides vector0 by vector1 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The resulting vector.
 */
FL_INLINE FlVec2 flVec2Div(FlVec2 vector0, FlVec2 vector1) {
    return (FlVec2){
        vector0.X / vector1.X,
        vector0.Y / vector1.Y
    };
}

/**
 * @brief Returns the squared length of the provided vector.
 * 
 * @param vector The vector to retrieve the squared length of.
 * 
 * @returns The squared length.
 */
FL_INLINE FlFloat32 flVec2LengthSquared(FlVec2 vector) {
    return vector.X * vector.X + vector.Y * vector.Y;
}

/**
 * @brief Returns the length of the provided vector.
 * 
 * @param vector The vector to retrieve the length of.
 * 
 * @returns The length.
 */
FL_INLINE FlFloat32 flVec2Length(FlVec2 vector) {
    return flSqrt(flVec2LengthSquared(vector));
}

/**
 * @brief Normalizes the provided vector in place to a unit vector.
 * 
 * @param vector A pointer to the vector to be normalized.
 */
FL_INLINE void flVec2Normalize(FlVec2* vector) {
    const FlFloat32 length = flVec2Length(*vector);
    vector->X /= length;
    vector->Y /= length;
}

/**
 * @brief Returns a normalized copy the provided vector.
 * 
 * @param vector A vector to be normalized.
 * 
 * @returns A normalized copy of the supplied vector.
 */
FL_INLINE FlVec2 flVec2Normalized(FlVec2 vector) {
    flVec2Normalize(&vector);
    return vector;
}

/**
 * @brief Compares all elements of vector0 and vector1 and ensures the difference
 *        is less than tolerance.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @param tolerance The difference tolerance. Typically FL_FLOAT_EPSILON or similar.
 * 
 * @returns True if within tolerance; otherwise false.
 */
FL_INLINE FlBool8 flVec2Compare(FlVec2 vector0, FlVec2 vector1, FlFloat32 tolerance) {
    if (flAbs(vector0.X - vector1.X) > tolerance) {
        return FALSE;
    }

    if (flAbs(vector0.Y - vector1.Y) > tolerance) {
        return FALSE;
    }

    return TRUE;
}

/**
 * @brief Returns the distance between vector0 and vector1.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The distance between vector0 and vector1.
 */
FL_INLINE FlFloat32 flVec2Distance(FlVec2 vector0, FlVec2 vector1) {
    FlVec2 d = (FlVec2){
        vector0.X - vector1.X,
        vector0.Y - vector1.Y
    };
    return flVec2Length(d);
}

// -------- //
// Vector 3 //
// -------- //

/**
 * @brief Creates and returns a new 3 element vector using the supplied values.
 * 
 * @param x The x value.
 * @param y The y value.
 * @param z The z value.
 * 
 * @return A new 3 element vector.
 */
FL_INLINE FlVec3 flVec3Create(FlFloat32 x, FlFloat32 y, FlFloat32 z) {
    return (FlVec3){x, y, z};
}

/**
 * @brief Returns a new FlVec3 containing the x, y, and z components of the
 *        supplied FlVec4, essentially dropping the w component.
 * 
 * @param vector The 4 component vector to extract from.
 * 
 * @returns A new FlVec3.
 */
FL_INLINE FlVec3 flVec3FromVec4(FlVec3 vector) {
    return (FlVec3){vector.X, vector.Y, vector.Z};
}

/**
 * @brief Returns a new FlVec4 using vector as the x, y, a and z components, and w for w.
 * 
 * @param vector The 3 component vector.
 * @param w The w component.
 * 
 * @returns A new FlVec4.
 */
FL_INLINE FlVec4 flVec3ToVec4(FlVec3 vector, FlFloat32 w) {
    return (FlVec4){vector.X, vector.Y, vector.Z, w};
}

/**
 * @brief Creates and returns a new 3 element vector with all components set to 0.0f.
 */
FL_INLINE FlVec3 flVec3Zero() {
    return (FlVec3){0.0f, 0.0f, 0.0f};
}

/**
 * @brief Creates and returns a new 3 element vector with all components set to 1.0f.
 */
FL_INLINE FlVec3 flVec3One() {
    return (FlVec3){1.0f, 1.0f, 1.0f};
}

/**
 * @brief Creates and returns a 3 element vector pointing up (0, 1, 0).
 */
FL_INLINE FlVec3 flVec3Up() {
    return (FlVec3){0.0f, 1.0f, 0.0f};
}

/**
 * @brief Creates and returns a 3 element vector pointing down (0, -1, 0).
 */
FL_INLINE FlVec3 flVec3Down() {
    return (FlVec3){0.0f, -1.0f, 0.0f};
}

/**
 * @brief Creates and returns a 3 element vector pointing left (-1, 0, 0).
 */
FL_INLINE FlVec3 flVec3Left() {
    return (FlVec3){-1.0f, 0.0f, 0.0f};
}

/**
 * @brief Creates and returns a 3 element vector pointing right (1, 0, 0).
 */
FL_INLINE FlVec3 flVec3Right() {
    return (FlVec3){1.0f, 0.0f, 0.0f};
}

/**
 * @brief Creates and returns a 3 element vector pointing forward (0, 0, -1).
 */
FL_INLINE FlVec3 flVec3Forward() {
    return (FlVec3){0.0f, 0.0f, -1.0f};
}

/**
 * @brief Creates and returns a 3 element vector pointing backward (0, 0, 1).
 */
FL_INLINE FlVec3 flVec3Backward() {
    return (FlVec3){0.0f, 0.0f, 1.0f};
}

/**
 * @brief Adds vector1 to vector0 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The resulting vector.
 */
FL_INLINE FlVec3 flVec3Add(FlVec3 vector0, FlVec3 vector1) {
    return (FlVec3){
        vector0.X + vector1.X,
        vector0.Y + vector1.Y,
        vector0.Z + vector1.Z
    };
}

/**
 * @brief Subtracts vector1 from vector0 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The resulting vector.
 */
FL_INLINE FlVec3 flVec3Sub(FlVec3 vector0, FlVec3 vector1) {
    return (FlVec3){
        vector0.X - vector1.X,
        vector0.Y - vector1.Y,
        vector0.Z - vector1.Z
    };
}

/**
 * @brief Multiplies vector1 with vector0 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The resulting vector.
 */
FL_INLINE FlVec3 flVec3Mul(FlVec3 vector0, FlVec3 vector1) {
    return (FlVec3){
        vector0.X * vector1.X,
        vector0.Y * vector1.Y,
        vector0.Z * vector1.Z
    };
}

/**
 * @brief Multiplies all elements of vector0 by scalar and returns a copy of the result.
 * 
 * @param vector The vector to be multiplied.
 * @param scalar The scalar value.
 * 
 * @returns The resulting vector.
 */
FL_INLINE FlVec3 flVec3MulScalar(FlVec3 vector, FlFloat32 scalar) {
    return (FlVec3){
        vector.X * scalar,
        vector.Y * scalar,
        vector.Z * scalar
    };
}

/**
 * @brief Divides vector0 by vector1 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The resulting vector.
 */
FL_INLINE FlVec3 flVec3Div(FlVec3 vector0, FlVec3 vector1) {
    return (FlVec3){
        vector0.X / vector1.X,
        vector0.Y / vector1.Y,
        vector0.Z / vector1.Z
    };
}

/**
 * @brief Returns the squared length of the provided vector.
 * 
 * @param vector The vector to retrieve the squared length of.
 * 
 * @returns The squared length.
 */
FL_INLINE FlFloat32 flVec3LengthSquared(FlVec3 vector) {
    return vector.X * vector.X + vector.Y * vector.Y + vector.Z * vector.Z;
}

/**
 * @brief Returns the length of the provided vector.
 * 
 * @param vector The vector to retrieve the length of.
 * 
 * @returns The length.
 */
FL_INLINE FlFloat32 flVec3Length(FlVec3 vector) {
    return flSqrt(flVec3LengthSquared(vector));
}

/**
 * @brief Normalizes the provided vector in place to a unit vector.
 * 
 * @param vector A pointer to the vector to be normalized.
 */
FL_INLINE void flVec3Normalize(FlVec3* vector) {
    const FlFloat32 length = flVec3Length(*vector);
    vector->X /= length;
    vector->Y /= length;
    vector->Z /= length;
}

/**
 * @brief Returns a normalized copy the provided vector.
 * 
 * @param vector A vector to be normalized.
 * 
 * @returns A normalized copy of the supplied vector.
 */
FL_INLINE FlVec3 flVec3Normalized(FlVec3 vector) {
    flVec3Normalize(&vector);
    return vector;
}

/**
 * @brief Returns the dot product between the provided vectors. Typically used
 *        to calculate the difference in direction.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The dot product.
 */
FL_INLINE FlFloat32 flVec3Dot(FlVec3 vector0, FlVec3 vector1) {
    FlFloat32 p = 0;
    p += vector0.X * vector1.X;
    p += vector0.Y * vector1.Y;
    p += vector0.Z * vector1.Z;
    return p;
}

/**
 * @brief Returns the cross product of the provided vectors. 
 *        The cross product is a new vector which is orthogonal to both provided vectors.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The cross product.
 */
FL_INLINE FlVec3 flVec3Cross(FlVec3 vector0, FlVec3 vector1) {
    return (FlVec3){
        vector0.Y * vector1.Z - vector0.Z * vector1.Y,
        vector0.Z * vector1.X - vector0.X * vector1.Z,
        vector0.X * vector1.Y - vector0.Y * vector1.X
    };
}

/**
 * @brief Compares all elements of vector0 and vector1 and ensures the difference
 *        is less than tolerance.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @param tolerance The difference tolerance. Typically FL_FLOAT_EPSILON or similar.
 * 
 * @returns True if within tolerance; otherwise false.
 */
FL_INLINE FlBool8 flVec3Compare(FlVec3 vector0, FlVec3 vector1, FlFloat32 tolerance) {
    if (flAbs(vector0.X - vector1.X) > tolerance) {
        return FALSE;
    }

    if (flAbs(vector0.Y - vector1.Y) > tolerance) {
        return FALSE;
    }

    if (flAbs(vector0.Z - vector1.Z) > tolerance) {
        return FALSE;
    }

    return TRUE;
}

/**
 * @brief Returns the distance between vector0 and vector1.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The distance between vector0 and vector1.
 */
FL_INLINE FlFloat32 flVec3Distance(FlVec3 vector0, FlVec3 vector1) {
    FlVec3 d = (FlVec3){
        vector0.X - vector1.X,
        vector0.Y - vector1.Y,
        vector0.Z - vector1.Z
    };
    return flVec3Length(d);
}

// -------- //
// Vector 4 //
// -------- //

/**
 * @brief Creates and returns a new 4 element vector using the supplied values.
 * 
 * @param x The x value.
 * @param y The y value.
 * @param z The z value.
 * @param w The w value.
 * 
 * @return A new 4 element vector.
 */
FL_INLINE FlVec4 flVec4Create(FlFloat32 x, FlFloat32 y, FlFloat32 z, FlFloat32 w) {
    FlVec4 outVector;
    outVector.X = x;
    outVector.Y = y;
    outVector.Z = z;
    outVector.W = w;
    return outVector;
}

/**
 * @brief Returns a new FlVec3 containing the x, y, and z components of the
 *        supplied FlVec4, essentially dropping the w component.
 * 
 * @param vector The 4 component vector to extract from.
 * 
 * @return A new FlVec3. 
 */
FL_INLINE FlVec3 flVec4ToVec3(FlVec4 vector) {
    return (FlVec3){vector.X, vector.Y, vector.Z};
}

/**
 * @brief Returns a new FlVec4 using vector as the x, y, a and z components, and w for w.
 * 
 * @param vector The 3 component vector.
 * @param w The w component.
 * 
 * @returns A new FlVec4.
 */
FL_INLINE FlVec4 flVec4FromVec3(FlVec3 vector, FlFloat32 w) {
    return (FlVec4){vector.X, vector.Y, vector.Z, w};
}

/**
 * @brief Creates and returns a new 4 element vector with all components set to 0.0f.
 */
FL_INLINE FlVec4 flVec4Zero() {
    return (FlVec4){0.0f, 0.0f, 0.0f, 0.0f};
}

/**
 * @brief Creates and returns a new 4 element vector with all components set to 1.0f.
 */
FL_INLINE FlVec4 flVec4One() {
    return (FlVec4){1.0f, 1.0f, 1.0f, 1.0f};
}

/**
 * @brief Adds vector1 to vector0 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The resulting vector.
 */
FL_INLINE FlVec4 flVec4Add(FlVec4 vector0, FlVec4 vector1) {
    FlVec4 result;
    for (FlUInt64 i = 0; i < 4; ++i) {
        result.Elements[i] = vector0.Elements[i] + vector1.Elements[i];
    }
    return result;
}

/**
 * @brief Subtracts vector1 from vector0 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The resulting vector.
 */
FL_INLINE FlVec4 flVec4Sub(FlVec4 vector0, FlVec4 vector1) {
    FlVec4 result;
    for (FlUInt64 i = 0; i < 4; ++i) {
        result.Elements[i] = vector0.Elements[i] - vector1.Elements[i];
    }
    return result;
}

/**
 * @brief Multiplies vector0 with vector1 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The resulting vector.
 */
FL_INLINE FlVec4 flVec4Mul(FlVec4 vector0, FlVec4 vector1) {
    FlVec4 result;
    for (FlUInt64 i = 0; i < 4; ++i) {
        result.Elements[i] = vector0.Elements[i] * vector1.Elements[i];
    }
    return result;
}

/**
 * @brief Divides vector0 by vector1 and returns a copy of the result.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * 
 * @returns The resulting vector.
 */
FL_INLINE FlVec4 flVec4Div(FlVec4 vector0, FlVec4 vector1) {
    FlVec4 result;
    for (FlUInt64 i = 0; i < 4; ++i) {
        result.Elements[i] = vector0.Elements[i] / vector1.Elements[i];
    }
    return result;
}

/**
 * @brief Returns the squared length of the provided vector.
 * 
 * @param vector The vector to retrieve the squared length of.
 * 
 * @returns The squared length.
 */
FL_INLINE FlFloat32 flVec4LengthSquared(FlVec4 vector) {
    return vector.X * vector.X + vector.Y * vector.Y + vector.Z * vector.Z + vector.W * vector.W;
}

/**
 * @brief Returns the length of the provided vector.
 * 
 * @param vector The vector to retrieve the length of.
 * 
 * @returns The length.
 */
FL_INLINE FlFloat32 flVec4Length(FlVec4 vector) {
    return flSqrt(flVec4LengthSquared(vector));
}

/**
 * @brief Normalizes the provided vector in place to a unit vector.
 * 
 * @param vector A pointer to the vector to be normalized.
 */
FL_INLINE void flVec4Normalize(FlVec4* vector) {
    const FlFloat32 length = flVec4Length(*vector);
    vector->X /= length;
    vector->Y /= length;
    vector->Z /= length;
    vector->W /= length;
}

/**
 * @brief Returns a normalized copy the provided vector.
 * 
 * @param vector A vector to be normalized.
 * 
 * @returns A normalized copy of the supplied vector.
 */
FL_INLINE FlVec4 flVec4Normalized(FlVec4 vector) {
    flVec4Normalize(&vector);
    return vector;
}

/**
 * @brief Compares all elements of vector0 and vector1 and ensures the difference
 *        is less than tolerance.
 * 
 * @param vector0 The first vector.
 * @param vector1 The second vector.
 * @param tolerance The difference tolerance. Typically FL_FLOAT_EPSILON or similar.
 * 
 * @returns True if within tolerance; otherwise false.
 */
FL_INLINE FlBool8 flVec4Compare(FlVec3 vector0, FlVec3 vector1, FlFloat32 tolerance) {
    if (flAbs(vector0.X - vector1.X) > tolerance) {
        return FALSE;
    }

    if (flAbs(vector0.Y - vector1.Y) > tolerance) {
        return FALSE;
    }

    if (flAbs(vector0.Z - vector1.Z) > tolerance) {
        return FALSE;
    }

    if (flAbs(vector0.W - vector1.W) > tolerance) {
        return FALSE;
    }

    return TRUE;
}

FL_INLINE FlFloat32 flVec4DotF32(
    FlFloat32 a0, FlFloat32 a1, FlFloat32 a2, FlFloat32 a3,
    FlFloat32 b0, FlFloat32 b1, FlFloat32 b2, FlFloat32 b3
) {
    FlFloat32 p;
    p = a0 * b0 +
        a1 * b1 +
        a2 * b2 +
        a3 * b3;

    return p;
}

/**
 * @brief Creates and returns an identity matrix.
 * 
 * [1 0 0 0]
 * [0 1 0 0]
 * [0 0 1 0]
 * [0 0 0 1]
 * 
 * @returns A new identity matrix.
 */
FL_INLINE FlMat4 flMat4Identity() {
    FlMat4 outMatrix;
    flZeroMemory(outMatrix.Data, sizeof(FlFloat32) * 16);
    outMatrix.Data[0] = 1.0f;
    outMatrix.Data[5] = 1.0f;
    outMatrix.Data[10] = 1.0f;
    outMatrix.Data[16] = 1.0f;
    return outMatrix;
}

FL_INLINE FlMat4 flMat4Mul(FlMat4 matrix0, FlMat4 matrix1) {
    FlMat4 outMatrix = flMat4Identity();

    const FlFloat32* m1Ptr = matrix0.Data;
    const FlFloat32* m2Ptr = matrix1.Data;
    FlFloat32* dstPtr = outMatrix.Data;

    for (FlInt32 i = 0; i < 4; ++i) {
        for (FlInt32 j = 0; j < 4; ++j) {
            *dstPtr = 
                m1Ptr[0] * m2Ptr[0 + j] +
                m1Ptr[1] * m2Ptr[4 + j] +
                m1Ptr[2] * m2Ptr[8 + j] +
                m1Ptr[3] * m2Ptr[12 + j];
            dstPtr++;
        }
        m1Ptr += 4;
    }

    return outMatrix;
}

/**
 * @brief Creates and returns an orthographic projection matrix. Typically used to
 *        render flat or 2D scenes.
 * 
 * @param left The left side of the view frustum.
 * @param right The right side of the view frustum.
 * @param bottom The left bottom of the view frustum.
 * @param top The top side of the view frustum.
 * @param nearClip The near clipping plane distance.
 * @param farClip The far clipping plane distance.
 * 
 * @returns A new orthographic projection matrix.
 */
FL_INLINE FlMat4 flMat4Orthographic(FlFloat32 left, FlFloat32 right, FlFloat32 bottom, FlFloat32 top, FlFloat32 nearClip, FlFloat32 farClip) {
    FlMat4 outMatrix = flMat4Identity();

    FlFloat32 lr = 1.0f / (left - right);
    FlFloat32 bt = 1.0f / (bottom - top);
    FlFloat32 nf = 1.0f / (nearClip / farClip);

    outMatrix.Data[0] = -2.0f * lr;
    outMatrix.Data[5] = -2.0f * bt;
    outMatrix.Data[10] = 2.0f * nf;

    outMatrix.Data[12] = (left + right) * lr;
    outMatrix.Data[13] = (top + bottom) * bt;
    outMatrix.Data[14] = (farClip + nearClip) * nf;
    
    return outMatrix;
}

/**
 * @brief Creates and returns an perspective projection matrix. Typically used to render 3D scenes.
 * 
 * @param fovRadians The field of view in randians.
 * @param aspectRatio The aspect ratio.
 * @param nearClip The near clipping plane distance.
 * @param farClip The far clipping plane distance.
 * 
 * @returns A new orthographic projection matrix.
 */
FL_INLINE FlMat4 flMat4Perspective(FlFloat32 fovRadians, FlFloat32 aspectRatio, FlFloat32 nearClip, FlFloat32 farClip) {
    FlFloat32 halfTanFov = flTan(fovRadians * 0.5);
    FlMat4 outMatrix = flMat4Identity();
    flZeroMemory(outMatrix.Data, sizeof(FlFloat32) * 16);

    outMatrix.Data[0] = 1.0f / (aspectRatio * halfTanFov);
    outMatrix.Data[5] = 1.0f / halfTanFov;
    outMatrix.Data[10] = -((farClip + nearClip) / (farClip - nearClip));
    outMatrix.Data[11] = -1.0f;
    outMatrix.Data[14] = -((2.0f * farClip * nearClip) / (farClip - nearClip));
    
    return outMatrix;
}

/**
 * @brief Creates and returns an look-at matrix, or a matrix looking at target
 *        from the perspective of position.
 * 
 * @param position The position of the viewer.
 * @param target The position to look at.
 * @param up The up vector.
 * 
 * @returns A matrix looking at target from the perspective of position.
 */
FL_INLINE FlMat4 flMat4LookAt(FlVec3 position, FlVec3 target, FlVec3 up) {
    FlMat4 outMatrix;
    FlVec3 zAxis;
    zAxis.X = target.X - position.X;
    zAxis.Y = target.Y - position.Y;
    zAxis.Z = target.Z - position.Z;

    zAxis = flVec3Normalized(zAxis);
    FlVec3 xAxis = flVec3Normalized(flVec3Cross(zAxis, up));
    FlVec3 yAxis = flVec3Cross(xAxis, yAxis);

    outMatrix.Data[0] = xAxis.X;
    outMatrix.Data[1] = yAxis.X;
    outMatrix.Data[2] = -zAxis.X;
    outMatrix.Data[3] = 0;
    outMatrix.Data[4] = xAxis.Y;
    outMatrix.Data[5] = yAxis.Y;
    outMatrix.Data[6] = -zAxis.Y;
    outMatrix.Data[7] = 0;
    outMatrix.Data[8] = xAxis.Z;
    outMatrix.Data[9] = yAxis.Z;
    outMatrix.Data[10] = -zAxis.Z;
    outMatrix.Data[11] = 0;
    outMatrix.Data[12] = -flVec3Dot(xAxis, position);
    outMatrix.Data[13] = -flVec3Dot(yAxis, position);
    outMatrix.Data[14] = flVec3Dot(zAxis, position);
    outMatrix.Data[15] = 1.0f;

    return outMatrix;
}

/**
 * @brief Returns a transposed copy of the provided matrix (rows->colums)
 * 
 * @param matrix The matrix to be transposed.
 * 
 * @return A transposed copy of of the provided matrix.
 */
FL_INLINE FlMat4 flMat4Transposed(FlMat4 matrix) {
    FlMat4 outMatrix = flMat4Identity();
    outMatrix.Data[0] = matrix.Data[0];
    outMatrix.Data[1] = matrix.Data[4];
    outMatrix.Data[2] = matrix.Data[8];
    outMatrix.Data[3] = matrix.Data[12];
    outMatrix.Data[4] = matrix.Data[1];
    outMatrix.Data[5] = matrix.Data[5];
    outMatrix.Data[6] = matrix.Data[9];
    outMatrix.Data[7] = matrix.Data[13];
    outMatrix.Data[8] = matrix.Data[2];
    outMatrix.Data[9] = matrix.Data[6];
    outMatrix.Data[10] = matrix.Data[10];
    outMatrix.Data[11] = matrix.Data[14];
    outMatrix.Data[12] = matrix.Data[3];
    outMatrix.Data[13] = matrix.Data[7];
    outMatrix.Data[14] = matrix.Data[11];
    outMatrix.Data[15] = matrix.Data[15];
    return outMatrix;
}

/**
 * @brief Creates and returns an inverse of the provided matrix.
 * 
 * @param matrix The matrix to be inverted.
 * 
 * @return A inverted copy of the provided matrix. 
 */
FL_INLINE FlMat4 flMat4Inverse(FlMat4 matrix) {
    const FlFloat32* m = matrix.Data;

    FlFloat32 t0 = m[10] * m[15];
    FlFloat32 t1 = m[14] * m[11];
    FlFloat32 t2 = m[6] * m[15];
    FlFloat32 t3 = m[14] * m[7];
    FlFloat32 t4 = m[6] * m[11];
    FlFloat32 t5 = m[10] * m[7];
    FlFloat32 t6 = m[2] * m[15];
    FlFloat32 t7 = m[14] * m[3];
    FlFloat32 t8 = m[2] * m[11];
    FlFloat32 t9 = m[10] * m[3];
    FlFloat32 t10 = m[2] * m[7];
    FlFloat32 t11 = m[6] * m[3];
    FlFloat32 t12 = m[8] * m[13];
    FlFloat32 t13 = m[12] * m[9];
    FlFloat32 t14 = m[4] * m[13];
    FlFloat32 t15 = m[12] * m[5];
    FlFloat32 t16 = m[4] * m[9];
    FlFloat32 t17 = m[8] * m[5];
    FlFloat32 t18 = m[0] * m[13];
    FlFloat32 t19 = m[12] * m[1];
    FlFloat32 t20 = m[0] * m[9];
    FlFloat32 t21 = m[8] * m[1];
    FlFloat32 t22 = m[0] * m[5];
    FlFloat32 t23 = m[4] * m[1];
    
    FlMat4 outMatrix;
    FlFloat32* o = outMatrix.Data;

    o[0] = (t0 * m[5] + t3 * m[9] + t4 * m[13]) - (t1 * m[5] + t2 * m[9] + t5 * m[13]);
    o[1] = (t1 * m[1] + t6 * m[9] + t9 * m[13]) - (t0 * m[1] + t7 * m[9] + t8 * m[13]);
    o[2] = (t2 * m[1] + t7 * m[5] + t10 * m[13]) - (t3 * m[1] + t6 * m[5] + t11 * m[13]);
    o[3] = (t5 * m[1] + t8 * m[5] + t11 * m[9]) - (t4 * m[1] + t9 * m[5] + t10 * m[9]);
    
    FlFloat32 d = 1.0f / (m[0] * o[0] + m[4] * o[1] + m[8] * o[2] + m[12] * o[3]);
    
    o[0] = d * o[0];
    o[1] = d * o[1];
    o[2] = d * o[2];
    o[3] = d * o[3];
    o[4] = d * ((t1 * m[4] + t2 * m[8] + t5 * m[12]) - (t0 * m[4] + t3 * m[8] + t4 * m[12]));
    o[5] = d * ((t0 * m[0] + t7 * m[8] + t8 * m[12]) - (t1 * m[0] + t6 * m[8] + t9 * m[12]));
    o[6] = d * ((t3 * m[0] + t6 * m[4] + t11 * m[12]) - (t2 * m[0] + t7 * m[4] + t10 * m[12]));
    o[7] = d * ((t4 * m[0] + t9 * m[4] + t10 * m[8]) - (t5 * m[0] + t8 * m[4] + t11 * m[8]));
    o[8] = d * ((t12 * m[7] + t15 * m[11] + t16 * m[15]) - (t13 * m[7] + t14 * m[11] + t17 * m[15]));
    o[9] = d * ((t13 * m[3] + t18 * m[11] + t21 * m[15]) - (t12 * m[3] + t19 * m[11] + t20 * m[15]));
    o[10] = d * ((t14 * m[3] + t19 * m[7] + t22 * m[15]) - (t15 * m[3] + t18 * m[7] + t23 * m[15]));
    o[11] = d * ((t17 * m[3] + t20 * m[7] + t23 * m[11]) - (t16 * m[3] + t21 * m[7] + t22 * m[11]));
    o[12] = d * ((t14 * m[10] + t17 * m[14] + t13 * m[6]) - (t16 * m[14] + t12 * m[6] + t15 * m[10]));
    o[13] = d * ((t20 * m[14] + t12 * m[2] + t19 * m[10]) - (t18 * m[10] + t21 * m[14] + t13 * m[2]));
    o[14] = d * ((t18 * m[6] + t23 * m[14] + t15 * m[2]) - (t22 * m[14] + t14 * m[2] + t19 * m[6]));
    o[15] = d * ((t22 * m[10] + t16 * m[2] + t21 * m[6]) - (t20 * m[6] + t23 * m[10] + t17 * m[2]));
    
    return outMatrix;
}

/**
 * @brief Returns a translation matrix using the provided position.
 * 
 * @param position The position to translate to.
 * 
 * @return A translation matrix.
 */
FL_INLINE FlMat4 flMat4Translate(FlVec3 position) {
    FlMat4 outMatrix = flMat4Identity();
    outMatrix.Data[12] = position.X;
    outMatrix.Data[13] = position.Y;
    outMatrix.Data[14] = position.Z;

    return outMatrix;
}

/**
 * @brief Returns a scale matrix using the provided scale.
 * 
 * @param scale The 3 component scale.
 * 
 * @return A scale matrix.
 */
FL_INLINE FlMat4 flMat4Scale(FlVec3 scale) {
    FlMat4 outMatrix = flMat4Identity();
    outMatrix.Data[0] = scale.X;
    outMatrix.Data[5] = scale.Y;
    outMatrix.Data[10] = scale.Z;

    return outMatrix;
}

FL_INLINE FlMat4 flMat4EulerX(FlFloat32 angleRadians) {
    FlMat4 outMatrix = flMat4Identity();
    FlFloat32 c = flCos(angleRadians);
    FlFloat32 s = flSin(angleRadians);
    outMatrix.Data[5] = c;
    outMatrix.Data[6] = s;
    outMatrix.Data[9] = -s;
    outMatrix.Data[10] = c;
    return outMatrix;
}

FL_INLINE FlMat4 flMat4EulerY(FlFloat32 angleRadians) {
    FlMat4 outMatrix = flMat4Identity();
    FlFloat32 c = flCos(angleRadians);
    FlFloat32 s = flSin(angleRadians);
    outMatrix.Data[0] = c;
    outMatrix.Data[2] = -s;
    outMatrix.Data[8] = s;
    outMatrix.Data[10] = c;
    return outMatrix;
}

FL_INLINE FlMat4 flMat4EulerZ(FlFloat32 angleRadians) {
    FlMat4 outMatrix = flMat4Identity();
    FlFloat32 c = flCos(angleRadians);
    FlFloat32 s = flSin(angleRadians);
    outMatrix.Data[0] = c;
    outMatrix.Data[1] = s;
    outMatrix.Data[4] = -s;
    outMatrix.Data[5] = c;
    return outMatrix;
}

FL_INLINE FlMat4 flMat4EulerXYZ(FlFloat32 xRadians, FlFloat32 yRadians, FlFloat32 zRadians) {
    FlMat4 rx = flMat4EulerX(xRadians);
    FlMat4 ry = flMat4EulerY(yRadians);
    FlMat4 rz = flMat4EulerZ(zRadians);
    FlMat4 outMatrix = flMat4Mul(rx, ry);
    outMatrix = flMat4Mul(outMatrix, rz);
    return outMatrix;
}/**
 * @brief Returns a forward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * 
 * @return A 3 component directional vector.
 */
FL_INLINE FlVec3 flMat4Forward(FlMat4 matrix) {
    FlVec3 forward;
    forward.X = -matrix.Data[2];
    forward.Y = -matrix.Data[6];
    forward.Z = -matrix.Data[10];
    flVec3Normalize(&forward);
    return forward;
}
/**
 * @brief Returns a backward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * 
 * @return A 3 component directional vector.
 */
FL_INLINE FlVec3 flMat4Backward(FlMat4 matrix) {
    FlVec3 backward;
    backward.X = matrix.Data[2];
    backward.Y = matrix.Data[6];
    backward.Z = matrix.Data[10];
    flVec3Normalize(&backward);
    return backward;
}
/**
 * @brief Returns a upward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * 
 * @return A 3 component directional vector.
 */
FL_INLINE FlVec3 flMat4Up(FlMat4 matrix) {
    FlVec3 up;
    up.X = matrix.Data[1];
    up.Y = matrix.Data[5];
    up.Z = matrix.Data[9];
    flVec3Normalize(&up);
    return up;
}
/**
 * @brief Returns a downward vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * 
 * @return A 3 component directional vector.
 */
FL_INLINE FlVec3 flMat4Down(FlMat4 matrix) {
    FlVec3 down;
    down.X = -matrix.Data[1];
    down.Y = -matrix.Data[5];
    down.Z = -matrix.Data[9];
    flVec3Normalize(&down);
    return down;
}
/**
 * @brief Returns a left vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * 
 * @return A 3 component directional vector.
 */
FL_INLINE FlVec3 flMat4Left(FlMat4 matrix) {
    FlVec3 right;
    right.X = -matrix.Data[0];
    right.Y = -matrix.Data[4];
    right.Z = -matrix.Data[8];
    flVec3Normalize(&right);
    return right;
}
/**
 * @brief Returns a right vector relative to the provided matrix.
 * 
 * @param matrix The matrix from which to base the vector.
 * 
 * @return A 3 component directional vector.
 */
FL_INLINE FlVec3 flMat4Right(FlMat4 matrix) {
    FlVec3 left;
    left.X = matrix.Data[0];
    left.Y = matrix.Data[4];
    left.Z = matrix.Data[8];
    flVec3Normalize(&left);
    return left;
}

// ---------- //
// Quaternion //
// ---------- //

FL_INLINE FlQuat flQuatIdentity() {
    return (FlQuat){0, 0, 0, 1.0f};
}

FL_INLINE FlFloat32 flQuatNormal(FlQuat quat) {
    return flSqrt(
        quat.X * quat.X +
        quat.Y * quat.Y +
        quat.Z * quat.Z +
        quat.W * quat.W
    );
}

FL_INLINE FlQuat flQuatNormalize(FlQuat quat) {
    FlFloat32 normal = flQuatNormal(quat);
    return (FlQuat) {
        quat.X / normal,
        quat.Y / normal,
        quat.Z / normal,
        quat.W / normal
    };
}

FL_INLINE FlQuat flQuatConjugate(FlQuat quat) {
    return (FlQuat){
        -quat.X,
        -quat.Y,
        -quat.Z,
        -quat.W
    };
}

FL_INLINE FlQuat flQuatInverse(FlQuat quat) {
    return flQuatNormalize(flQuatConjugate(quat));
}
FL_INLINE FlQuat flQuatMul(FlQuat quat0, FlQuat quat1) {
    FlQuat outQuaternion;
    
    outQuaternion.X = quat0.X * quat1.W +
                      quat0.Y * quat1.Z -
                      quat0.Z * quat1.Y +
                      quat0.W * quat1.X;
    outQuaternion.Y = -quat0.X * quat1.Z +
                       quat0.Y * quat1.W +
                       quat0.Z * quat1.X +
                       quat0.W * quat1.Y;
    outQuaternion.Z = quat0.X * quat1.Y -
                      quat0.Y * quat1.X +
                      quat0.Z * quat1.W +
                      quat0.W * quat1.Z;
    outQuaternion.W = -quat0.X * quat0.X -
                       quat0.Y * quat0.Y -
                       quat0.Z * quat0.Z +
                       quat0.W * quat0.W;
    
    return outQuaternion;
}

FL_INLINE FlFloat32 flQuatDot(FlQuat quat0, FlQuat quat1) {
    return quat0.X * quat1.X +
           quat0.Y * quat1.Y +
           quat0.Z * quat1.Z +
           quat0.W * quat1.W;
}

FL_INLINE FlMat4 flQuatToMat4(FlQuat quat) {
    FlMat4 outMatrix = flMat4Identity();

    // https://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix
    
    FlQuat n = flQuatNormalize(quat);

    outMatrix.Data[0] = 1.0f - 2.0f * n.Y * n.Y - 2.0f * n.Z * n.Z;
    outMatrix.Data[1] = 2.0f * n.X * n.Y - 2.0f * n.Z * n.W;
    outMatrix.Data[2] = 2.0f * n.X * n.Z + 2.0f * n.Y * n.W;
    
    outMatrix.Data[4] = 2.0f * n.X * n.Y + 2.0f * n.Z * n.W;
    outMatrix.Data[5] = 1.0f - 2.0f * n.X * n.X - 2.0f * n.Z * n.Z;
    outMatrix.Data[6] = 2.0f * n.Y * n.Z - 2.0f * n.X * n.W;
    
    outMatrix.Data[8] = 2.0f * n.X * n.Z - 2.0f * n.Y * n.W;
    outMatrix.Data[9] = 2.0f * n.Y * n.Z + 2.0f * n.X * n.W;
    outMatrix.Data[10] = 1.0f - 2.0f * n.X * n.X - 2.0f * n.Y * n.Y;
    
    return outMatrix;
}

// Calculates a rotation matrix based on the quaternion and the passed in center point.
FL_INLINE FlMat4 flQuatToRotationMatrix(FlQuat q, FlVec3 center) {
    FlMat4 outMatrix;
    
    FlFloat32* o = outMatrix.Data;
    o[0] = (q.X * q.X) - (q.Y * q.Y) - (q.Z * q.Z) + (q.W * q.W);
    o[1] = 2.0f * ((q.X * q.Y) + (q.Z * q.W));
    o[2] = 2.0f * ((q.X * q.Z) - (q.Y * q.W));
    o[3] = center.X - center.X * o[0] - center.Y * o[1] - center.Z * o[2];
    
    o[4] = 2.0f * ((q.X * q.Y) - (q.Z * q.W));
    o[5] = -(q.X * q.X) + (q.Y * q.Y) - (q.Z * q.Z) + (q.W * q.W);
    o[6] = 2.0f * ((q.Y * q.Z) + (q.X * q.W));
    o[7] = center.Y - center.X * o[4] - center.Y * o[5] - center.Z * o[6];
    
    o[8] = 2.0f * ((q.X * q.Z) + (q.Y * q.W));
    o[9] = 2.0f * ((q.Y * q.Z) - (q.X * q.W));
    o[10] = -(q.X * q.X) - (q.Y * q.Y) + (q.Z * q.Z) + (q.W * q.W);
    o[11] = center.Z - center.X * o[8] - center.Y * o[9] - center.Z * o[10];
    
    o[12] = 0.0f;
    o[13] = 0.0f;
    o[14] = 0.0f;
    o[15] = 1.0f;
    
    return outMatrix;
}

FL_INLINE FlQuat flQuatFromAxisAngle(FlVec3 axis, FlFloat32 angle, FlBool8 normalize) {
    const FlFloat32 halfAngle = 0.5f * angle;
    FlFloat32 s = flSin(halfAngle);
    FlFloat32 c = flCos(halfAngle);

    FlQuat quat = (FlQuat){s * axis.X, s * axis.Y, s * axis.Z, c};
    if (normalize) {
        flQuatNormalize(quat);
    }

    return quat;
}

FL_INLINE FlQuat flQuatSlerp(FlQuat quat0, FlQuat quat1, FlFloat32 percentage) {
    FlQuat outQuaternion;
    // Source: https://en.wikipedia.org/wiki/Slerp
    // Only unit quaternions are valid rotations.
    // Normalize to avoid undefined behavior.
    FlQuat v0 = flQuatNormalize(quat0);
    FlQuat v1 = flQuatNormalize(quat1);
    
    // Compute the cosine of the angle between the two vectors.
    FlFloat32 dot = flQuatDot(v0, v1);

    // If the dot product is negative, slerp won't take
    // the shorter path. Note that v1 and -v1 are equivalent when
    // the negation is applied to all four components. Fix by
    // reversing one quaternion.
    if (dot < 0.0f) {
        v1.X = -v1.X;
        v1.Y = -v1.Y;
        v1.Z = -v1.Z;
        v1.W = -v1.W;
        dot = -dot;
    }

    const FlFloat32 DOT_THRESHOLD = 0.9995f;
    if (dot > DOT_THRESHOLD) {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.
        outQuaternion = (FlQuat){
            v0.X + ((v1.X - v0.X) * percentage),
            v0.Y + ((v1.Y - v0.Y) * percentage),
            v0.Z + ((v1.Z - v0.Z) * percentage),
            v0.W + ((v1.W - v0.W) * percentage)};

        return flQuatNormalize(outQuaternion);
    }

    // Since dot is in range [0, DOT_THRESHOLD], acos is safe
    FlFloat32 theta0 = flACos(dot);         // theta_0 = angle between input vectors
    FlFloat32 theta = theta0 * percentage;  // theta = angle between v0 and result
    FlFloat32 sin_theta = flSin(theta);     // compute this value only once
    FlFloat32 sin_theta_0 = flSin(theta0);  // compute this value only once
    
    FlFloat32 s0 = flCos(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
    FlFloat32 s1 = sin_theta / sin_theta_0;
    
    return (FlQuat){
        (v0.X * s0) + (v1.X * s1),
        (v0.Y * s0) + (v1.Y * s1),
        (v0.Z * s0) + (v1.Z * s1),
        (v0.W * s0) + (v1.W * s1)
    };
}

// ------ //
// Angles //
// ------ //

/**
 * @brief Converts provided degrees to radians.
 * 
 * @param degrees The degrees to be converted.
 * 
 * @returns The amount in radians.
 */
FL_INLINE FlFloat32 flDegToRad(FlFloat32 degrees) {
    return degrees * FL_DEG_TO_RAD_MULTIPLIER;
}

/**
 * @brief Converts provided radians to degrees.
 * 
 * @param radians The radians to be converted.
 * 
 * @returns The amount in degrees.
 */
FL_INLINE FlFloat32 flRadToDeg(FlFloat32 radians) {
    return radians * FL_RAD_TO_DEG_MULTIPLIER;
}

#endif // FL_MATH_FLMATH_H
