#pragma once

#include <Windows.h>

namespace base {

#define E_ASSERT(cond) if ((cond)) {} else { __debugbreak(); }

} // namespace base