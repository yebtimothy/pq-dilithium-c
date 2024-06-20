#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include "randombytes.h"

void randombytes(uint8_t *out, size_t outlen) {
  int fd;
  ssize_t r;
  size_t offset = 0;

  fd = open("/dev/urandom", O_RDONLY);
  if (fd == -1) {
    return;
  }

  while (offset < outlen) {
    r = read(fd, out + offset, outlen - offset);
    if (r <= 0) {
      break;
    }
    offset += r;
  }
  close(fd);
}
