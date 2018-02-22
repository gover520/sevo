/*
 *  vfs.h
 *
 *  copyright (c) 2018 Xiongfei Shi
 *
 *  author: Xiongfei Shi <jenson.shixf(a)gmail.com>
 *  license: Apache-2.0
 */

#ifndef __VFS_H__
#define __VFS_H__

#include <mclib.h>

#ifdef __cplusplus
extern "C" {
#endif

    #define FILETYPE_FILE           0
    #define FILETYPE_DIR            1
    #define FILETYPE_SYMLINK        2
    #define FILETYPE_OTHER          3

    typedef struct vfile_t {
        int     need_free;
        struct PHYSFS_File  *file;
    } vfile_t;

    typedef struct vfinfo_t {
        int         type;
        long long   size;
        long long   modtime;
        long long   createtime;
    } vfinfo_t;

    int vfs_init(const char *argv0);
    void vfs_deinit();

    int vfs_identity(const char *name, int addpath);

    int vfs_mount(const char *dir, const char *mount, int addpath);
    int vfs_mount_buffer(const void *buffer, int size, const char *name, const char *mount, int addpath);
    int vfs_unmount(const char *dir);

    int vfs_mkdir(const char *dir);
    int vfs_remove(const char *file);
    int vfs_info(const char *file, vfinfo_t *info);

    const char *vfs_realdir(const char *file);
    char **vfs_files(const char *dir);
    void vfs_freelist(void *lv);

    mc_sstr_t vfs_read(const char *file, int size);
    int vfs_write(const char *file, const void *data, int size);

    vfile_t *vfopen(vfile_t *fp, const char *filename, const char *mode);
    void vfclose(vfile_t *fp);

    long long vfsize(vfile_t *fp);
    long long vftell(vfile_t *fp);
    int vfseek(vfile_t *fp, long long pos);

    int vfeof(vfile_t *fp);
    int vfflush(vfile_t *fp);

    int vfread(vfile_t *fp, void *data, int size);
    int vfwrite(vfile_t *fp, const void *data, int size);

    const char *homedir(void);
    const char *identdir(void);
    const char *extdir(void);

#ifdef __cplusplus
};
#endif

#endif  /* __VFS_H__ */
