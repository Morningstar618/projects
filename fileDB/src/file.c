#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <Winsock2.h>

#include "file.h"
#include "common.h"

int open_db_file(char *filepath) {
	int fd = open(filepath, O_RDWR, 0644);	
	if (fd == -1) {
		perror("open");
		return STATUS_ERROR;
	}

	return fd;
}

int create_db_file(char *filepath) {
	int fd = open(filepath, O_RDONLY);
	if (fd != -1) {
		printf("\nFile already exists!\n");
		close(fd);
		return STATUS_ERROR;
	}

	fd = open(filepath, O_RDWR | O_CREAT, 0644);	
	if (fd == -1) {
		perror("open");
		return STATUS_ERROR;
	}

	return fd;
}
