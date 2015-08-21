/*
** $Id: ldo.h,v 2.22 2015/05/22 17:48:19 roberto Exp $
** Stack and Call structure of Lua
** See Copyright Notice in lua.h
*/

#ifndef ldo_h
#define ldo_h

#include "lua/lobject.h"
#include "lua/lstate.h"
#include "lua/lzio.h"

#define luaD_checkstack(L, n)        \
  if (L->stack_last - L->top <= (n)) \
    luaD_growstack(L, n);            \
  else                               \
    condmovestack(L);

#define incr_top(L)        \
  {                        \
    L->top++;              \
    luaD_checkstack(L, 0); \
  }

#define savestack(L, p) ((char *)(p) - (char *)L->stack)
#define restorestack(L, n) ((TValue *)((char *)L->stack + (n)))

/* type of protected functions, to be ran by 'runprotected' */
typedef void (*Pfunc)(lua_State *L, void *ud);

LUAI_FUNC int luaD_protectedparser(lua_State *L, ZIO *z, const char *name,
                                   const char *mode);
LUAI_FUNC void luaD_hook(lua_State *L, int event, int line);
LUAI_FUNC int luaD_precall(lua_State *L, StkId func, int nresults);
LUAI_FUNC void luaD_call(lua_State *L, StkId func, int nResults,
                         int allowyield);
LUAI_FUNC int luaD_pcall(lua_State *L, Pfunc func, void *u, ptrdiff_t oldtop,
                         ptrdiff_t ef);
LUAI_FUNC int luaD_poscall(lua_State *L, StkId firstResult, int nres);
LUAI_FUNC void luaD_reallocstack(lua_State *L, int newsize);
LUAI_FUNC void luaD_growstack(lua_State *L, int n);
LUAI_FUNC void luaD_shrinkstack(lua_State *L);

LUAI_FUNC l_noret luaD_throw(lua_State *L, int errcode);
LUAI_FUNC int luaD_rawrunprotected(lua_State *L, Pfunc f, void *ud);

#endif
