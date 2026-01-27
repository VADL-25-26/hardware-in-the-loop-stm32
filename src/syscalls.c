#include <stddef.h>

#include <sys/types.h>
#include <errno.h>


void _init(void) {}
void _fini(void) {}


void _exit(int status)
{
    (void)status;
    while (1);
}

int _write(int fd, char *ptr, int len)
{
    (void)fd;
    (void)ptr;
    return len;
}

int _read(int fd, char *ptr, int len)
{
    (void)fd;
    (void)ptr;
    (void)len;
    return 0;
}

int _close(int fd)
{
    (void)fd;
    return -1;
}

int _lseek(int fd, int ptr, int dir)
{
    (void)fd;
    (void)ptr;
    (void)dir;
    return 0;
}

void *_sbrk(ptrdiff_t incr)
{
    (void)incr;
    errno = ENOMEM;
    return (void *)-1;
}


