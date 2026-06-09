#include "fatfs_stub.h"

int fatfs_mount(const char *path)
{
    (void)path;

    return 0;
}

int fatfs_unmount(const char *path)
{
    (void)path;

    return 0;
}
