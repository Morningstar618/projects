#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void main() {
  int fd = open("gg", O_RDONLY);
  struct stat f_stat = {0};

  fstat(fd, &f_stat);

  char *data = (char *)calloc(1, f_stat.st_size + 1);

  read(fd, data, f_stat.st_size);
  data[f_stat.st_size] = '\0';

  printf("%s\n", data);
}
