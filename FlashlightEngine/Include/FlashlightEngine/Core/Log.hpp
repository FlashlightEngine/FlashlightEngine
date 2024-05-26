#pragma once

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/chrono.h>

#include <cstdlib>

#define FL_LOG(message)                                                   \
    fmt::print(fg(fmt::color::green) | fmt::emphasis::bold, "[LOG] "); \
    fmt::print(message);                                               \
    fmt::print("\n");

#define FL_WARN(message)                                                     \
    fmt::print(fg(fmt::color::gold) | fmt::emphasis::bold, "[WARNING] "); \
    fmt::print(message);                                                  \
    fmt::print(" at {0}:{1}", __FILE__, __LINE__);                        \
    fmt::print("\n");

#define FL_ERROR(message)                                                 \
    fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "[ERROR] "); \
    fmt::print(message);                                               \
    fmt::print(" at {0}:{1}", __FILE__, __LINE__);                     \
    fmt::print("\n");

#define FL_THROW(message)                                                            \
    fmt::print(fg(fmt::color::dark_red) | fmt::emphasis::bold, "[FATAL ERROR] "); \
    fmt::print(message);                                                          \
    fmt::print(" at {0}:{1}", __FILE__, __LINE__);                                \
    fmt::print("\n");                                                             \
    exit(EXIT_FAILURE);
