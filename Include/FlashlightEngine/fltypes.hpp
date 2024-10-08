// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in LICENSE

#pragma once

#include <cstdint>

// Engine type definitions
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using size = u64;

using f32 = float;
using f64 = double;

// Engine structures
struct EngineStats {
    f32 FrameTime;
    i32 TriangleCount;
    i32 DrawCallCount;
    f32 SceneUpdateTime;
    f32 MeshDrawTime;
};
