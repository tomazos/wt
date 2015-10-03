#pragma once

#include "lua/limits.h"
#include "lua/state.h"

#define api_incr_top(L)                                   \
  {                                                       \
    L->top++;                                             \
    api_check(L, L->top <= L->ci->top, "stack overflow"); \
  }

#define adjustresults(L, nres)                                             \
  {                                                                        \
    if ((nres) == LUA_MULTRET && L->ci->top < L->top) L->ci->top = L->top; \
  }

#define api_checknelems(L, n)                \
  api_check(L, (n) < (L->top - L->ci->func), \
            "not enough elements in the "    \
            "stack")
