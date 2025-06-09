#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 		0x4c4c4144
#define HEADER_VERSION		0x1

struct __attribute__(( packed )) dbheader_t {
	unsigned int magic;
	unsigned short version;
	unsigned short count;
	unsigned int filesize;
};

struct __attribute__(( packed )) employee_t {
	char name[256];
	char address[256];
	unsigned int hours;
};

int create_db_header(struct dbheader_t **headerOut);
int validate_db_header(int fd, struct dbheader_t **headerOut);
int output_file(int fd, struct dbheader_t *, struct employee_t *);
int read_employees(int fd, struct dbheader_t *dbheaderOut, struct employee_t **employeesOut);
int list_employees(struct dbheader_t *, struct employee_t *);

#endif
