// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef FL_CORE_FLSTRING_HPP
#define FL_CORE_FLSTRING_HPP

#include "FlashlightEngine/Prerequisites.hpp"
#include "FlashlightEngine/Export.hpp"

namespace Fl {

FL_API char* StringDuplicate(const char* str);

FL_API UInt64 StringLength(const char* str);

// Case-sensitive string comparison.
FL_API bool StringsEqual(const char* lhs, const char* rhs);

}

#endif // FL_CORE_FLSTRING_HPP
