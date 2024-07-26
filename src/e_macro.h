#pragma once

#ifdef E_DEBUG_BUILD
    #define E_DEBUG_OP(x) x
#else
    #define E_DEBUG_OP(x)
#endif

#define E_MAX(a, b) ((a) > (b) ? (a) : (b))
#define E_MIN(a, b) ((a) < (b) ? (a) : (b))