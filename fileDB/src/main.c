#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "file.h"
#include "parse.h"
#include "common.h"

void print_usage(char *argv[]);
void free_p(struct dbheader_t *, struct employee_t *);
void close_fd(int db_fd);
int add_employee(struct dbheader_t *, struct employee_t *, char *);

int main(int argc, char *argv[]) {
	char *filepath = NULL;
	char *addstring = NULL;
	bool newfile = false;

	int db_fd = -1;
	struct dbheader_t *db_header = NULL;
	struct employee_t *employees = NULL;

	int c = 0;
	while ((c = getopt(argc, argv, "nf:a:")) != -1) {
		switch (c) {
			case 'n':
				newfile = true;
				break;
			
			case 'f':
				filepath = optarg;
				break;

			case 'a':
				addstring = optarg;
				break;

			case '?':
				printf("[!] Unknown argument -%c\n", c);
				break;

			default:
				return -1;
		}
	}

	if (filepath == NULL) {
		printf("File path is a required argument\n");
		print_usage(argv);

		return 0;
	}

	if (newfile) {
		db_fd = create_db_file(filepath);
		if (db_fd == STATUS_ERROR) {
			printf("\tUnable to create database file\n");
			return STATUS_ERROR;
		}

		if (create_db_header(&db_header) != STATUS_SUCCESS) {
			printf("\tUnable to create database header\n");
			free_p(db_header, employees);
			close_fd(db_fd);
			return STATUS_ERROR;
		}


	} else {
		db_fd = open_db_file(filepath);
		if (db_fd == STATUS_ERROR) {
			printf("\tUnable to open database file\n");
			return STATUS_ERROR;
		}

		if (validate_db_header(db_fd, &db_header) != STATUS_SUCCESS) {
			printf("\tDatabase Validation Failed!\n");
			free_p(db_header, employees);
			close_fd(db_fd);
			return STATUS_ERROR;
		}
	}


	if (read_employees(db_fd, db_header, &employees) != STATUS_SUCCESS) {
		printf("\tUnable to read employee data from the database file\n");
		free_p(db_header, employees);
		close_fd(db_fd);
		return STATUS_ERROR;
	}

	if (addstring) {
		db_header->count++;
		
		if (realloc(employees, db_header->count * sizeof(struct employee_t)) == NULL) {
			perror("realloc");
			free_p(db_header, employees);
			close_fd(db_fd);
			return STATUS_ERROR;
		}

		add_employee(db_header, employees, addstring);
	}

	output_file(db_fd, db_header, employees);

	free_p(db_header, employees);
	close_fd(db_fd);
	return 0;
}

int add_employee(struct dbheader_t *dbheader, struct employee_t *employees, char *addstring) {
	char *name = strtok(addstring, "|");
	char *address = strtok(NULL, "|");
	char *hours = strtok(NULL, "|");

	printf("[*] ADDED RECORD:\n\tname: %s\n\taddress: %s\n\thours: %s\n", name, address, hours);
	
	strncpy(employees[dbheader->count - 1].name, name, sizeof(employees[dbheader->count - 1].name));
	strncpy(employees[dbheader->count - 1].address, address, sizeof(employees[dbheader->count - 1].address));
	employees[dbheader->count - 1].hours = atoi(hours);
	
	return STATUS_SUCCESS;
}

void close_fd(int db_fd) {
	close(db_fd);
}

void free_p(struct dbheader_t *dbheader, struct employee_t *employees) {
	free(dbheader);
	free(employees);
}

void print_usage(char *argv[]) {
	printf("\n[*] Usage: %s -n -f <filepath>\n", argv[0]);
	printf("\t-n - create a new database file\n");
	printf("\t-f - (required) path to database file\n");
}

