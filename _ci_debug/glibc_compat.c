/*
 * 临时探针：把静态 OpenSSL 3 引入的 glibc 2.25/2.28 符号降级到 2.2.5。
 * 仅用于验证方案，不进入正式代码。
 */
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <sys/syscall.h>
#include <unistd.h>

void explicit_bzero(void* ptr, size_t len)
{
    volatile unsigned char* p = (volatile unsigned char*)ptr;
    while (len-- > 0) {
        *p++ = 0;
    }
}

int getentropy(void* buffer, size_t length)
{
    if (length > 256) {
        errno = EIO;
        return -1;
    }
    long rc;
    do {
        rc = syscall(SYS_getrandom, buffer, length, 0);
    } while (rc == -1 && errno == EINTR);
    if (rc != (long)length) {
        if (rc != -1) {
            errno = EIO;
        }
        return -1;
    }
    return 0;
}

int fcntl64(int fd, int cmd, ...)
{
    va_list ap;
    va_start(ap, cmd);
    long arg = va_arg(ap, long);
    va_end(ap);
    return (int)syscall(SYS_fcntl, fd, cmd, arg);
}
