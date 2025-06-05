#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

extern int _end;

void _exit(int status) {
	while (1) {}
}

caddr_t _sbrk(int incr) {
	static unsigned char *heap = NULL;
	unsigned char *prev_heap;

	if (heap == NULL) {
		heap = (unsigned char *)&_end;
	}
	prev_heap = heap;
	heap += incr;

	return (caddr_t) prev_heap;
}

int _write(int file, char *ptr, int len) {
	return len;
}

int _read(int file, char *ptr, int len) {
	return 0;
}

int _close(int file) {
	return -1;
}

int _fstat(int file, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file) {
	return 1;
}

int _lseek(int file, int ptr, int dir) {
	return 0;
}

int _open(const char *name, int flags, int mode) {
	return -1;
}

int _kill(int pid, int sig) {
	errno = EINVAL;
	return -1;
}

int _getpid(void) {
	return 1;
}
