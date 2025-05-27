#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

struct database_header_t {
	unsigned short version;
	unsigned short employees;
	unsigned int file_size;
};

int main(int argc, char *argv[]) {
	struct database_header_t head = {0};
	struct stat dbStat = {0};

	if (argc < 2) {
		printf("[!] Usage: %s <filename>\n", argv[0]);
	}

	int fd = open(argv[1], O_RDWR | O_CREAT, 0644);
	if (fd == -1) {
		perror("open");
	}

	if (read(fd, &head, sizeof(head)) != sizeof(head)) {
		perror("read");
		close(fd);
		return -1;
	}

	printf("[INFO] DB VERSION: %u\n", head.version);
	printf("[INFO] DB EMPLOYEES: %u\n", head.employees);
	printf("[INFO] DB FILESIZE: %d\n", head.file_size);

	if (fstat(fd, &dbStat) < 0) {
		perror("fstat");
		close(fd);
		return -1;
	}

	printf("[INFO] File size reported by fstat: %u\n", dbStat.st_size);

	if (dbStat.st_size != head.file_size) {
		printf("\n[ERROR] DB FILE CORRUPTED!\n");
		close(fd);
		return -2;
	}
	

	close(fd);
	return 0;
}
