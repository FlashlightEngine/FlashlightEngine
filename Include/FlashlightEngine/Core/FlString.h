// Copyright (C) 2024 Jean "Pixfri" Letessier 
// This file is part of Flashlight Engine.
// For conditions of distribution and use, see copyright notice in Export.h

#pragma once

#ifndef FL_CORE_FLSTRING_H
#define FL_CORE_FLSTRING_H

#include "FlashlightEngine/Prerequisites.h"
#include "FlashlightEngine/Export.h"

FL_API char* flStringDuplicate(const char* str);

FL_API FlUInt64 flStringLength(const char* str);

#endif // FL_CORE_FLSTRING_H
