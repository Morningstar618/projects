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
int remove_employee(struct dbheader_t *, struct employee_t **, char *);
int update_hours(struct dbheader_t *, struct employee_t *, char *, char *);

int main(int argc, char *argv[]) {
	char *filepath = NULL;
	char *addstring = NULL;
	bool newfile = false;
	bool listemployees = false;
	bool help = false;
	char *removeemployee = NULL;
	char *updated_hours = NULL;
	char *employee = NULL;

	int db_fd = -1;
	struct dbheader_t *db_header = NULL;
	struct employee_t *employees = NULL;

	int c = 0;
	while ((c = getopt(argc, argv, "hnf:a:lr:u:e:")) != -1) {
		switch (c) {
			case 'h':
				help = true;
				break;

			case 'n':
				newfile = true;
				break;
			
			case 'f':
				filepath = optarg;
				break;

			case 'a':
				addstring = optarg;
				break;

			case 'l':
				listemployees = true;
				break;

			case 'r':
				removeemployee = optarg;
				break;
			
			case 'u':
				updated_hours = optarg;
				break;

			case 'e':
				employee = optarg;
				break;
			
			case '?':
				printf("[!] Unknown argument -%c\n", c);
				break;

			default:
				return -1;
		}
	}

	if (help) {
		print_usage(argv);
		return STATUS_SUCCESS;
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

	if (listemployees)
		list_employees(db_header, employees);


	if (removeemployee) {
		remove_employee(db_header, &employees, removeemployee);
		ftruncate(db_fd, db_header->filesize);	// required as the write syscall does not truncate the filesize
	}

	if (updated_hours) {
		update_hours(db_header, employees, updated_hours, employee);
	}

	output_file(db_fd, db_header, employees);

	free_p(db_header, employees);
	close_fd(db_fd);
	return 0;
}

int update_hours(struct dbheader_t *dbheader, struct employee_t *employees, char *updated_hours, char *employee) {
	if (dbheader->count == 0) {
		printf("[*] No employee in the database\n");
		return STATUS_ERROR;
	}

	int i = 0;
	for (; i < dbheader->count; i++)
		if (strcmp(employees[i].name, employee) == 0) break;
	
	if (i == dbheader->count) {
		printf("[*] Employee %s not found\n", employee);
		return STATUS_ERROR;
	}

	int hours = atoi(updated_hours);
	if (hours == 0 && updated_hours[0] != 48) {
		printf("[!] Received invalid input!\n");
		return STATUS_ERROR;
	}

	employees[i].hours = hours;
	return STATUS_SUCCESS;
}

int remove_employee(struct dbheader_t *dbheader, struct employee_t **employees, char *removeemployee) {
	if (dbheader->count == 0) {
		printf("[!] No employees in the Database\n");
		return STATUS_ERROR;
	}

	// One employee case	
	if (dbheader->count == 1) {
		free(*employees);
		*employees = NULL;
		
		dbheader->count--;
		dbheader->filesize = sizeof(struct dbheader_t);
		
		return STATUS_SUCCESS;
	}

	int i = 0;
	
	for (; i < dbheader->count; i++)
		if (strcmp((*employees)[i].name, removeemployee) == 0) break;

	if (i == dbheader->count) {
		printf("[*] Employee not found!\n");
		return STATUS_ERROR;
	}

	dbheader->count--;

	// Removing last employee
	if (i == dbheader->count) {
		*employees = realloc(*employees, dbheader->count * sizeof(struct employee_t));
		dbheader->filesize = sizeof(struct dbheader_t) + dbheader->count * sizeof(struct employee_t);
		
		return STATUS_SUCCESS;
	}

	// Removing first employee
	if (i == 0) {
		memmove(*employees, &(*employees)[1], dbheader->count * sizeof(struct employee_t));
		*employees = realloc(*employees, dbheader->count * sizeof(struct employee_t));
		dbheader->filesize = sizeof(struct dbheader_t) + dbheader->count * sizeof(struct employee_t);
	
		return STATUS_SUCCESS;
	}	

	// Removing middle employees
	strncpy((*employees)[i].name, (*employees)[i+1].name, sizeof((*employees)[i].name));
	strncpy((*employees)[i].address, (*employees)[i+1].address, sizeof((*employees)[i].address));
	(*employees[i]).hours = (*employees)[i+1].hours;

	*employees = realloc(*employees, dbheader->count * sizeof(struct employee_t));
	dbheader->filesize = sizeof(struct dbheader_t) + dbheader->count * sizeof(struct employee_t);

	return STATUS_SUCCESS;
}

int add_employee(struct dbheader_t *dbheader, struct employee_t *employees, char *addstring) {
	char *name = strtok(addstring, "|");
	char *address = strtok(NULL, "|");
	char *hours = strtok(NULL, "|");

	strncpy(employees[dbheader->count - 1].name, name, sizeof(employees[dbheader->count - 1].name));
	strncpy(employees[dbheader->count - 1].address, address, sizeof(employees[dbheader->count - 1].address));
	employees[dbheader->count - 1].hours = atoi(hours);
	
	return STATUS_SUCCESS;
}

void close_fd(int db_fd) {
	close(db_fd);
}

void free_p(struct dbheader_t *dbheader, struct employee_t *employees) {
	if (dbheader) free(dbheader);
	if (employees) free(employees);
}

void print_usage(char *argv[]) {
	printf("\n[*] Usage: %s -f <dbfilepath> <opts>\n", argv[0]);
	printf("\t-n - create a new database file\n");
	printf("\t-f - (required) path to database file\n");
	printf("\t-l - lists employees\n");
	printf("\t-a - add employee. For e.g. %s -f <dbfilepath> -a \"Foo|London|100\"\n", argv[0]);
	printf("\t-r - remove employee. For e.g. %s -f <dbfilepath> -r \"<Name>\"\n", argv[0]);
	printf("\t-h - help menu\n");
}

