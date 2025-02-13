#ifndef E2FSTOOL_H_INC
#define E2FSTOOL_H_INC

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sparse/sparse.h>
#include <assert.h>
#include <e2p/e2p.h>
#include <ext2fs/ext2fs.h>
#include <ext2fs/ext2fsP.h>

#include <private/android_filesystem_capability.h>

#define E2FSTOOL_VERSION "1.0.0"
#define E2FSTOOL_DATE "30-Mar-2023"

#if defined(__CYGWIN__) || defined(_WIN32)
#include <windows.h>
#define symlink xsymlink
#endif

#ifdef __MINGW32__
#define mkdir(p, m) mkdir(p)
#endif

#ifndef XATTR_SELINUX_SUFFIX
#define XATTR_SELINUX_SUFFIX "selinux"
#endif
#ifndef XATTR_CAPS_SUFFIX
#define XATTR_CAPS_SUFFIX "capability"
#endif

#define FILE_MODE_MASK 0x0FFF
#define FILE_READ_BUFLEN (1 << 26)
#define RESERVED_INODES_COUNT 0xA /* Excluding EXT2_ROOT_INO */
#define SYMLINK_I_BLOCK_MAX_SIZE 0x3D

struct inode_params {
    ext2_filsys fs;
    char *path;
    char *filename;
    __u8 *mountpoint;
    char *fs_path;
    char *se_path;
    errcode_t error;
};

#if defined(__CYGWIN__) || defined(_WIN32)
static int xsymlink(char *target, const char *file)
{
    int sz = -1;
    char buf[PATH_MAX * sizeof(WCHAR)];

    FILE *lnk = fopen(file, "wb");
    if (!lnk || fprintf(lnk, "!<symlink>\xff\xfe") < 0)
        return -1;

    sz = MultiByteToWideChar(CP_ACP, 0,
        target, -1, (LPWSTR)buf, sizeof(buf) / sizeof(WCHAR));
    sz *= sizeof(WCHAR);
    if (sz < 0 || fwrite(buf, 1, sz, lnk) != (size_t)sz) {
        sz = -1;
        goto err;
    }

    if (!SetFileAttributes(file, FILE_ATTRIBUTE_SYSTEM)) {
        sz = -1;
        goto err;
    }
    sz = 0;
err:
    fclose(lnk);
    return sz;
}
#endif
#endif /* E2FSTOOL_H_INC */
