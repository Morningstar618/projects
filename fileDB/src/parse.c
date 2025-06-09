#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include "common.h"
#include "parse.h"

int list_employees(struct dbheader_t *dbheader, struct employee_t *employees) {
	if (!dbheader->count) {
		printf("[*] No employees found in the database\n");
		return STATUS_SUCCESS;
	}
	
	for (int i = 0; i < dbheader->count; i++) {
		printf("Employee %d\n", i + 1);
		printf("\tName: %s\n", employees[i].name);
		printf("\tAddress: %s\n", employees[i].address);
		printf("\tHours: %d\n", employees[i].hours);	
	}
	
	return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbheader, struct employee_t **employeesOut) {
	if (fd < 0) {
		printf("\n[!] Bad FD - read_employees/parse.c\n");
		return STATUS_ERROR;
	}

	int count = dbheader->count;
	
	struct employee_t *employees = (struct employee_t *)calloc(count, sizeof(struct employee_t));
	if (employees == NULL) {
		printf("\n[!] Calloc failed - read_employees/parse.c\n");
		return STATUS_ERROR;
	}

	if (read(fd, employees, count * sizeof(struct employee_t)) == STATUS_ERROR) {
		perror("read");
		free(employees);
		return STATUS_ERROR;
	}
	
	for (int i=0; i<count; i++)
		employees[i].hours = ntohl(employees[i].hours);

	*employeesOut = employees;
	return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t *dbheader, struct employee_t *employees) {
	if (fd < 0) {
		printf("\n[!] BAD FD - update_db_file, file.c\n");
		return STATUS_ERROR;
	}

	int count = dbheader->count;

	dbheader->version = htons(dbheader->version);
	dbheader->count = htons(dbheader->count);
	dbheader->magic = htonl(dbheader->magic);
	dbheader->filesize = htonl(sizeof(struct dbheader_t) + sizeof(struct employee_t) * count);

	lseek(fd, 0, SEEK_SET); // Reseting FD's offset that was changed during db file read before writing data back to the fd
	
	if (write(fd, dbheader, sizeof(struct dbheader_t)) == STATUS_ERROR) {
		perror("write");
		return STATUS_ERROR;
	}
	
	if (!employees) return STATUS_SUCCESS;

	for (int i=0; i<count; i++) {
		employees[i].hours = htonl(employees[i].hours);
		if (write(fd, &employees[i], sizeof(struct employee_t)) == STATUS_ERROR) {
			perror("write");
			return STATUS_ERROR;
		}
	}
	
	return STATUS_SUCCESS;
}

int create_db_header(struct dbheader_t **headerOut) {
	struct dbheader_t *header = (struct dbheader_t *)calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		printf("\n[!] Calloc to create database header pointer failed\n");
		return STATUS_ERROR;
	}

	header->version = HEADER_VERSION;
	header->count = 0x0;
	header->magic = HEADER_MAGIC;
	header->filesize = sizeof(struct dbheader_t);

	*headerOut = header;

	return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
	if (fd < 0) {
		printf("\n[!] Invalid FD provided to validate database header\n");
		return STATUS_ERROR;
	}

	struct dbheader_t *header = (struct dbheader_t *)calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		printf("\n[!] Calloc call to create data header failed -- validate_db_header\n");
		return STATUS_ERROR;
	}

	if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		perror("read");
		free(header);
		return STATUS_ERROR;
	}

	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->magic = ntohl(header->magic);
	header->filesize = ntohl(header->filesize);

	if (header->version != HEADER_VERSION) {
		printf("\n[!] Improper database header version\n");
		free(header);
		return STATUS_ERROR;
	}


	if (header->magic != HEADER_MAGIC) {
		printf("\n[!] Improper database magic version\n");
		free(header);
		return STATUS_ERROR;
	}

	struct stat dbstat = {0};
	fstat(fd, &dbstat);
	if (dbstat.st_size != header->filesize) {
		printf("\n[!] Corrupted database\n");
		free(header);
		return STATUS_ERROR;
	}

	*headerOut = header;
	return STATUS_SUCCESS;
}
