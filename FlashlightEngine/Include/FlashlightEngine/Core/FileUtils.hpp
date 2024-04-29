#pragma once

#include "FlashlightEngine/pch.hpp"

namespace Flashlight {

inline std::vector<char> ReadBinary(const std::filesystem::path &filename);

#include "FileUtils.inl"

}
