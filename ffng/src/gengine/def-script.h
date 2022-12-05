#ifndef HEADER_DEF_SCRIPT_H
#define HEADER_DEF_SCRIPT_H

class Scripter;

#include "Log.h"

#include <stdexcept>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}


#if DANDAN
//NOTE: no one exception can be passed to "C" lua code
#define BEGIN_NOEXCEPTION try {
#define END_NOEXCEPTION \
} \
catch (std::exception &e) { \
    luaL_error(L, e.what()); \
} \
catch (...) { \
    luaL_error(L, "unknown exception"); \
}
#else
#define BEGIN_NOEXCEPTION {
#define END_NOEXCEPTION }
#endif

inline const char *script_getLeaderName() { return "script_leader"; }
extern Scripter *script_getLeader(lua_State *L);
extern int script_debugStack(lua_State *L);

extern int script_file_include(lua_State *L) throw();
extern int script_file_exists(lua_State *L) throw();


#endif
