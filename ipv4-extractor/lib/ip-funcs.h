#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/* Global Vars */
char *ip = "\0";

void append_char(char *str, char ch) {
	size_t len;
	len = sizeof(*str);

	char *temp = str;

	printf("%d--\n", len);


	ip = (char *)malloc(4);
	if (ip == NULL) {
		fprintf(stderr, "Failed to allocate memory to new string");
		exit(3);
	}

	ip = temp;

	//ip[len - 1] = 'h';
	//ip[len] = '\0';

	printf("%d\n", sizeof(*ip) * (strlen(ip)));

}

void parse_ipv4 (char *input_file) {
	FILE *fptr;
	char ch;

	fptr = fopen(input_file, "r");
	if (fptr == NULL) {
		fprintf(stderr, "Failed to open file.\n");
		exit(2);
	}

	while((ch = fgetc(fptr)) != EOF) {
		if (ch >=48 && ch <= 57 || ch == 46) {
			append_char("Hii", ch);
		}
	}

	printf("%s", ip);
}
