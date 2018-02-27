/*
 *  wrap_vfs.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include <mclib.h>
#include <string.h>
#include "wrap_vfs.h"
#include "common/vfs.h"
#include "common/logger.h"

static const char g_meta_vfile[] = { CODE_NAME "meta.vfile" };

#define luaX_checkvfile(L, idx)     (vfile_t *)luaL_checkudata(L, idx, g_meta_vfile)

static int w_load(lua_State *L, const char *name) {
    mc_sstr_t buffer;

    buffer = vfs_read(name, -1);
    if (!buffer) {
        LG_ERR("Can not load %s.", name);
        return luaL_error(L, "Can not load %s.", name);
    }

    luaX_loadbuffer(L, buffer, mc_sstr_length(buffer), name);
    mc_sstr_destroy(buffer);

    return 1;
}

static int w_loader(lua_State *L) {
    const char *modname = lua_tostring(L, 1);
    char *p, name[MC_MAX_PATH] = { 0 };
    int len;
    vfinfo_t stat;

    strcpy(name, modname);
    len = (int)strlen(name);

    for (p = name; *p; ++p) {
        if ('.' == (*p)) {
            (*p) = '/';
        }
    }

    strcpy(name + len, ".lua");
    if ((0 == vfs_info(name, &stat)) && (FILETYPE_DIR != stat.type)) {
        return w_load(L, name);
    }

    strcpy(name + len, "/init.lua");
    if ((0 == vfs_info(name, &stat)) && (FILETYPE_DIR != stat.type)) {
        return w_load(L, name);
    }

    lua_pushfstring(L, "no module %s.", modname);
    return 1;
}

static const char *g_libext[] = {
#if defined(_WIN32)
    ".dll"
#elif defined(__APPLE__)
    ".dylib", ".so"
#else
    ".so"
#endif
};

static int w_extloader(lua_State *L) {
    const char *modname = lua_tostring(L, 1);
    char *p, name[MC_MAX_PATH] = { 0 };
    char temp[MC_MAX_PATH] = { 0 };
    mc_dylib_t handle = NULL;
    lua_CFunction func;
    int i, len;
    vfinfo_t stat;

    strcpy(name, modname);
    len = (int)strlen(name);

    for (p = name; *p; ++p) {
        if ('.' == (*p)) {
            (*p) = '/';
        }
    }

    for (i = 0; i < MC_ARRAY_SIZE(g_libext); ++i) {
        strcpy(name + len, g_libext[i]);

        if ((0 != vfs_info(name, &stat)) || (FILETYPE_DIR == stat.type)) {
            continue;
        }

        sprintf(temp, "%s/%s", vfs_realdir(name), name);
        mc_path_format(temp, MC_PATHSEP);

        handle = mc_dl_handle(temp);
        if (handle) {
            break;
        }
    }

    if (!handle) {
        lua_pushfstring(L, "no module %s.", modname);
        return 1;
    }

    name[len] = '\0';

    for (p = name; *p; ++p) {
        if ('/' == (*p)) {
            (*p) = '_';
        }
    }
    sprintf(temp, "luaopen_%s", name);

    func = (lua_CFunction)mc_dl_symbol(handle, temp);

    if (!func) {
        lua_pushfstring(L, "C library '%s' is incompatible.", name);
        return 1;
    }

    lua_pushcfunction(L, func);
    return 1;
}

static int w_getcwd(lua_State *L) {
    char cwd[MC_MAX_PATH] = { 0 };
    lua_pushstring(L, mc_getcwd(cwd, sizeof(cwd)));
    return 1;
}

static int w_identity(lua_State *L) {
    const char *name = luaL_checkstring(L, 1);
    int addpath = luaX_optboolean(L, 2, 1);

    lua_pushboolean(L, 0 == vfs_identity(name, addpath));
    return 1;
}

static int w_mount(lua_State *L) {
    const char *dir = luaL_checkstring(L, 1);
    const char *mount = luaL_checkstring(L, 2);
    int addpath = luaX_optboolean(L, 3, 1);

    lua_pushboolean(L, 0 == vfs_mount(dir, mount, addpath));
    return 1;
}

static int w_unmount(lua_State *L) {
    const char *dir = luaL_checkstring(L, 1);
    lua_pushboolean(L, 0 == vfs_unmount(dir));
    return 1;
}

static int w_mkdir(lua_State *L) {
    const char *dir = luaL_checkstring(L, 1);
    lua_pushboolean(L, 0 == vfs_mkdir(dir));
    return 1;
}

static int w_remove(lua_State *L) {
    const char *file = luaL_checkstring(L, 1);
    lua_pushboolean(L, 0 == vfs_remove(file));
    return 1;
}

static int w_info(lua_State *L) {
    vfinfo_t info;
    const char *file = luaL_checkstring(L, 1);

    if (0 == vfs_info(file, &info)) {
        lua_createtable(L, 0, 4);

        if (FILETYPE_FILE == info.type) {
            lua_pushstring(L, "file");
        } else if (FILETYPE_DIR == info.type) {
            lua_pushstring(L, "directory");
        } else if (FILETYPE_SYMLINK == info.type) {
            lua_pushstring(L, "symlink");
        } else {
            lua_pushstring(L, "other");
        }
        lua_setfield(L, -2, "type");

        lua_pushinteger(L, info.size);
        lua_setfield(L, -2, "size");

        lua_pushinteger(L, info.modtime);
        lua_setfield(L, -2, "modtime");

        lua_pushinteger(L, info.createtime);
        lua_setfield(L, -2, "createtime");
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int w_realdir(lua_State *L) {
    const char *file = luaL_checkstring(L, 1);
    lua_pushstring(L, vfs_realdir(file));
    return 1;
}

static int w_files(lua_State *L) {
    const char *dir = luaL_checkstring(L, 1);
    char **files = vfs_files(dir);
    char **i;
    int n = 0;

    if (files) {
        for (i = files; NULL != *i; ++i) {
            n += 1;
        }

        lua_createtable(L, n, 0);

        for (i = files, n = 1; NULL != *i; ++i, ++n) {
            lua_pushstring(L, *i);
            lua_rawseti(L, -2, n);
        }

        vfs_freelist(files);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int w_read(lua_State *L) {
    const char *file = luaL_checkstring(L, 1);
    int size = (int)luaL_optinteger(L, 2, -1);
    mc_sstr_t data = vfs_read(file, size);

    if (data) {
        lua_pushlstring(L, data, mc_sstr_length(data));
        mc_sstr_destroy(data);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int w_write(lua_State *L) {
    size_t len = 0;
    const char *file = luaL_checkstring(L, 1);
    const char *data = luaL_checklstring(L, 2, &len);
    int size = (int)luaL_optinteger(L, 3, (lua_Integer)len);

    lua_pushboolean(L, 0 == vfs_write(file, data, size));

    return 1;
}

static int w_open(lua_State *L) {
    const char *file = luaL_checkstring(L, 1);
    const char *mode = luaL_checkstring(L, 2);
    vfile_t vf, *p;

    if (vfopen(&vf, file, mode)) {
        p = (vfile_t *)luaX_newuserdata(L, g_meta_vfile, sizeof(vfile_t));
        *p = vf;
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int w_vf__gc(lua_State *L) {
    vfile_t *vf = luaX_checkvfile(L, -1);
    if (vf) {
        vfclose(vf);
    }
    return 0;
}

static int w_vf_close(lua_State *L) {
    vfile_t *vf = luaX_checkvfile(L, 1);
    vfclose(vf);
    return 0;
}

static int w_vf_size(lua_State *L) {
    vfile_t *vf = luaX_checkvfile(L, 1);
    lua_pushinteger(L, (lua_Integer)vfsize(vf));
    return 1;
}

static int w_vf_tell(lua_State *L) {
    vfile_t *vf = luaX_checkvfile(L, 1);
    lua_pushinteger(L, (lua_Integer)vftell(vf));
    return 1;
}

static int w_vf_seek(lua_State *L) {
    vfile_t *vf = luaX_checkvfile(L, 1);
    long long pos = (long long)luaL_checkinteger(L, 2);
    lua_pushboolean(L, 0 == vfseek(vf, pos));
    return 1;
}

static int w_vf_eof(lua_State *L) {
    vfile_t *vf = luaX_checkvfile(L, 1);
    lua_pushboolean(L, 1 == vfeof(vf));
    return 1;
}

static int w_vf_flush(lua_State *L) {
    vfile_t *vf = luaX_checkvfile(L, 1);
    lua_pushboolean(L, 0 == vfflush(vf));
    return 1;
}

static int w_vf_read(lua_State *L) {
    vfile_t *vf = luaX_checkvfile(L, 1);
    int size = (int)luaL_optinteger(L, 2, -1);
    mc_sstr_t data;

    if (size <= 0) {
        size = (int)vfsize(vf);
    }

    data = mc_sstr_from_buffer(NULL, size);
    size = vfread(vf, data, size);

    lua_pushlstring(L, data, size);
    mc_sstr_destroy(data);

    return 1;
}

static int w_vf_write(lua_State *L) {
    size_t len = 0;
    vfile_t *vf = luaX_checkvfile(L, 1);
    const char *data = luaL_checklstring(L, 2, &len);
    int size = (int)luaL_optinteger(L, 3, (lua_Integer)len);

    lua_pushinteger(L, vfwrite(vf, data, size));
    return 1;
}

int luaopen_sevo_vfs(lua_State* L) {
    luaL_Reg mod_vfs[] = {
        { "getcwd", w_getcwd },
        { "identity", w_identity },
        { "mount", w_mount },
        { "unmount", w_unmount },
        { "mkdir", w_mkdir },
        { "remove", w_remove },
        { "info", w_info },
        { "realdir", w_realdir },
        { "files", w_files },
        { "read", w_read },
        { "write", w_write },
        { "open", w_open },
        { NULL, NULL }
    };
    luaL_Reg meta_vfile[] = {
        { "__gc", w_vf__gc },
        { "close", w_vf_close },
        { "size", w_vf_size },
        { "tell", w_vf_tell },
        { "seek", w_vf_seek },
        { "eof", w_vf_eof },
        { "flush", w_vf_flush },
        { "read", w_vf_read },
        { "write", w_vf_write },
        { NULL, NULL }
    };

    luaX_register_searcher(L, w_loader);
    luaX_register_searcher(L, w_extloader);

    luaX_register_module(L, "vfs", mod_vfs);
    luaX_register_type(L, g_meta_vfile, meta_vfile);

    return 0;
}
