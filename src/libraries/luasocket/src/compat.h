#ifndef COMPAT_H
#define COMPAT_H

#if LUA_VERSION_NUM > 501
#define luaL_checkint(L,n)  ((int)luaL_checkinteger(L, (n)))
#endif

#endif
