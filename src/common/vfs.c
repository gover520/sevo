/*
 *  vfs.c
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#include <physfs.h>
#include <stdio.h>
#include "vfs.h"
#include "version.h"

static char g_base_dir[MC_MAX_PATH] = { 0 };
static char g_ident_dir[MC_MAX_PATH] = { 0 };
static char g_ext_dir[MC_MAX_PATH] = { 0 };

int vfs_init(const char *argv0) {
    if (PHYSFS_isInit()) {
        return 0;
    }

    if (!PHYSFS_init(argv0)) {
        return -1;
    }

    PHYSFS_permitSymbolicLinks(1);

    sprintf(g_base_dir, "%s/." CODE_NAME, mc_home_path());
    mc_path_format(g_base_dir, MC_PATHSEP);

    if (!mc_path_exist(g_base_dir)) {
        mc_path_mkdir(g_base_dir);
    }

    sprintf(g_ext_dir, "%s/extensions", g_base_dir);
    mc_path_format(g_ext_dir, MC_PATHSEP);

    if (!mc_path_exist(g_ext_dir)) {
        mc_path_mkdir(g_ext_dir);
    }

    vfs_identity("", 1);
    vfs_mount(g_ext_dir, "/_extensions_", 1);

    return 0;
}

void vfs_deinit() {
    if (PHYSFS_isInit()) {
        PHYSFS_deinit();
    }
}

int vfs_identity(const char *name, int addpath) {
    if (!PHYSFS_isInit()) {
        return -1;
    }

    if (g_ident_dir[0]) {
        PHYSFS_unmount(g_ident_dir);
        PHYSFS_setWriteDir(NULL);
    }

    if (name && name[0]) {
        sprintf(g_ident_dir, "%s/datasets/%s", g_base_dir, name);
    } else {
        sprintf(g_ident_dir, "%s/datasets", g_base_dir);
    }
    mc_path_format(g_ident_dir, MC_PATHSEP);

    if (!mc_path_exist(g_ident_dir)) {
        mc_path_mkdir(g_ident_dir);
    }

    PHYSFS_setWriteDir(g_ident_dir);

    if (!PHYSFS_mount(g_ident_dir, NULL, addpath)) {
        PHYSFS_setWriteDir(NULL);
        return -1;
    }

    return 0;
}

int vfs_mount(const char *dir, const char *mount, int addpath) {
    if (!PHYSFS_isInit()) {
        return -1;
    }

    if (!PHYSFS_mount(dir, mount, addpath)) {
        return -1;
    }
    return 0;
}

int vfs_mount_buffer(const void *buffer, int size, const char *name, const char *mount, int addpath) {
    if (!PHYSFS_isInit()) {
        return -1;
    }

    if (!PHYSFS_mountMemory(buffer, size, NULL, name, mount, addpath)) {
        return -1;
    }
    return 0;
}

int vfs_unmount(const char *dir) {
    if (!PHYSFS_isInit()) {
        return -1;
    }

    if (!PHYSFS_unmount(dir)) {
        return -1;
    }
    return 0;
}

int vfs_mkdir(const char *dir) {
    if (!PHYSFS_isInit()) {
        return -1;
    }

    if (!PHYSFS_mkdir(dir)) {
        return -1;
    }
    return 0;
}

int vfs_remove(const char *file) {
    if (!PHYSFS_isInit()) {
        return -1;
    }

    if (!PHYSFS_delete(file)) {
        return -1;
    }
    return 0;
}

int vfs_info(const char *file, vfinfo_t *info) {
    PHYSFS_Stat stat = { 0 };

    if (!PHYSFS_isInit()) {
        return -1;
    }

    if (!PHYSFS_stat(file, &stat)) {
        return -1;
    }

    info->size = stat.filesize;
    info->modtime = stat.modtime;
    info->createtime = stat.createtime;

    if (PHYSFS_FILETYPE_REGULAR == stat.filetype) {
        info->type = FILETYPE_FILE;
    } else if (PHYSFS_FILETYPE_DIRECTORY == stat.filetype) {
        info->type = FILETYPE_DIR;
    } else if (PHYSFS_FILETYPE_SYMLINK == stat.filetype) {
        info->type = FILETYPE_SYMLINK;
    } else {
        info->type = FILETYPE_OTHER;
    }

    return 0;
}

const char *vfs_realdir(const char *file) {
    if (!PHYSFS_isInit()) {
        return NULL;
    }
    return PHYSFS_getRealDir(file);
}

char **vfs_files(const char *dir) {
    if (!PHYSFS_isInit()) {
        return NULL;
    }

    return PHYSFS_enumerateFiles(dir);
}

void vfs_freelist(void *lv) {
    PHYSFS_freeList(lv);
}

mc_sstr_t vfs_read(const char *file, int size) {
    vfile_t *fp;
    mc_sstr_t data;

    fp = vfopen(NULL, file, "r");

    if (!fp) {
        return NULL;
    }

    if (size <= 0) {
        size = (int)vfsize(fp);
    }

    data = mc_sstr_from_buffer(NULL, size);
    size = vfread(fp, data, size);

    mc_sstr_range(data, 0, size);
    vfclose(fp);

    return data;
}

int vfs_write(const char *file, const void *data, int size) {
    vfile_t *fp;

    fp = vfopen(NULL, file, "w");

    if (!fp) {
        return -1;
    }

    vfwrite(fp, data, size);
    vfclose(fp);

    return 0;
}

vfile_t *vfopen(vfile_t *fp, const char *filename, const char *mode) {
    PHYSFS_File *handle = NULL;

    if (!PHYSFS_isInit()) {
        return NULL;
    }

    if (('r' == mode[0]) && (!PHYSFS_exists(filename))) {
        return NULL;
    }

    if ((('w' == mode[0]) || ('a' == mode[0])) && (!PHYSFS_getWriteDir())) {
        return NULL;
    }

    if ('r' == mode[0]) {
        handle = PHYSFS_openRead(filename);
    } else if ('w' == mode[0]) {
        handle = PHYSFS_openWrite(filename);
    } else if ('a' == mode[0]) {
        handle = PHYSFS_openAppend(filename);
    }

    if (!handle) {
        return NULL;
    }

    if (!fp) {
        fp = (vfile_t *)mc_malloc(sizeof(struct vfile_t));
        fp->need_free = 1;
    } else {
        fp->need_free = 0;
    }

    fp->file = handle;

    return fp;
}

void vfclose(vfile_t *fp) {
    if (fp->file) {
        PHYSFS_close(fp->file);
        fp->file = NULL;
    }

    if (fp->need_free) {
        mc_free(fp);
    }
}

long long vfsize(vfile_t *fp) {
    return PHYSFS_fileLength(fp->file);
}

long long vftell(vfile_t *fp) {
    return PHYSFS_tell(fp->file);
}

int vfseek(vfile_t *fp, long long pos) {
    return !PHYSFS_seek(fp->file, (PHYSFS_uint64)pos) ? -1 : 0;
}

int vfeof(vfile_t *fp) {
    return PHYSFS_eof(fp->file) ? 1 : 0;
}

int vfflush(vfile_t *fp) {
    return !PHYSFS_flush(fp->file) ? -1 : 0;
}

int vfread(vfile_t *fp, void *data, int size) {
    return (int)PHYSFS_readBytes(fp->file, data, (PHYSFS_uint64)size);
}

int vfwrite(vfile_t *fp, const void *data, int size) {
    return (int)PHYSFS_writeBytes(fp->file, data, (PHYSFS_uint64)size);
}

const char *homedir(void) {
    return g_base_dir;
}

const char *identdir(void) {
    return g_ident_dir;
}

const char *extdir(void) {
    return g_ext_dir;
}
