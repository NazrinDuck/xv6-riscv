#include "../kernel/types.h"

#include "../kernel/stat.h"
#include "../user/user.h"

#include <stdarg.h>

#define FILE_BUF_SZ 0x400

struct file_ptr {
  int fd;
  char buffer[FILE_BUF_SZ];
  uint64 head;
  uint64 size;
};

struct file_ptr stdptrs[3] = {
    {.fd = 0, .head = 0, .size = FILE_BUF_SZ},
    {.fd = 1, .head = 0, .size = FILE_BUF_SZ},
    {.fd = 2, .head = 0, .size = FILE_BUF_SZ},
};

struct file_ptr *stdin = stdptrs;
struct file_ptr *stdout = stdptrs + 1;
struct file_ptr *stderr = stdptrs + 2;

static char digits[] = "0123456789ABCDEF";

static void fputc(struct file_ptr *fp, char c) {
  fp->buffer[fp->head++] = c;

  if (fp->head > fp->size || c == '\n') {
    fflush(fp);
  }
}

static void fwrite(struct file_ptr *fp, char *restrict s, int n) {
  for (char *c = s; c < &s[n]; c++) {
    fputc(fp, *c);
  }
}

// static void putc(int fd, char c) { write(fd, &c, 1); }

static void printint(struct file_ptr *fp, long long xx, int base, int sgn) {
  char buf[20];
  int i, neg;
  unsigned long long x;

  neg = 0;
  if (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while ((x /= base) != 0);
  if (neg)
    buf[i++] = '-';

  while (--i >= 0)
    fputc(fp, buf[i]);
}

static void printptr(struct file_ptr *fp, uint64 x) {
  int i;
  fputc(fp, '0');
  fputc(fp, 'x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    fputc(fp, digits[x >> (sizeof(uint64) * 8 - 4)]);
}

int setnbuf(struct file_ptr *fp, int n) {
  n = n > FILE_BUF_SZ ? FILE_BUF_SZ : n;

  if (!fp || n < 0) {
    return -1;
  }

  fp->size = n;

  return n;
}

int fflush(struct file_ptr *fp) {
  int num = fp->head;
  if (num == 0) {
    return 0;
  }

  fp->head = 0;

  return write(fp->fd, fp->buffer, num);
}
// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void vprintf(struct file_ptr *fp, const char *fmt, va_list ap) {
  char *s;
  int c0, c1, c2, i, state;

  state = 0;
  for (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    if (state == 0) {
      if (c0 == '%') {
        state = '%';
      } else {
        fputc(fp, c0);
      }
    } else if (state == '%') {
      c1 = c2 = 0;
      if (c0)
        c1 = fmt[i + 1] & 0xff;
      if (c1)
        c2 = fmt[i + 2] & 0xff;
      if (c0 == 'd') {
        printint(fp, va_arg(ap, int), 10, 1);
      } else if (c0 == 'l' && c1 == 'd') {
        printint(fp, va_arg(ap, uint64), 10, 1);
        i += 1;
      } else if (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fp, va_arg(ap, uint64), 10, 1);
        i += 2;
      } else if (c0 == 'u') {
        printint(fp, va_arg(ap, uint32), 10, 0);
      } else if (c0 == 'l' && c1 == 'u') {
        printint(fp, va_arg(ap, uint64), 10, 0);
        i += 1;
      } else if (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fp, va_arg(ap, uint64), 10, 0);
        i += 2;
      } else if (c0 == 'x') {
        printint(fp, va_arg(ap, uint32), 16, 0);
      } else if (c0 == 'l' && c1 == 'x') {
        printint(fp, va_arg(ap, uint64), 16, 0);
        i += 1;
      } else if (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fp, va_arg(ap, uint64), 16, 0);
        i += 2;
      } else if (c0 == 'p') {
        printptr(fp, va_arg(ap, uint64));
      } else if (c0 == 'c') {
        fputc(fp, va_arg(ap, uint32));
      } else if (c0 == 's') {
        if ((s = va_arg(ap, char *)) == 0)
          s = "(null)";
        fwrite(fp, s, strlen(s));
      } else if (c0 == '%') {
        fputc(fp, '%');
      } else {
        // Unknown % sequence.  Print it to draw attention.
        fputc(fp, '%');
        fputc(fp, c0);
      }

      state = 0;
    }
  }
}

void fprintf(struct file_ptr *fp, const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprintf(fp, fmt, ap);
}

void printf(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  vprintf(stdout, fmt, ap);
}
