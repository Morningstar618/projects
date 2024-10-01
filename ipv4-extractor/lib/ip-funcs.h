#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char *append_char(char *str, char ch) {
	size_t len;
	len = strlen(str);

	char *newStr = (char *)malloc((len + 1));
	if (newStr == NULL) {
		fprintf(stderr, "Failed to allocate memory to new string");
		exit(3);
	}

	strcpy(newStr, str);

	if (len == 0) {
		newStr[len] == ch;
		newStr[len+1] == '\0';
	} else {
		newStr[len - 1] = ch;
		newStr[len] = '\0';
	}

	return newStr;

}

void parse_ipv4 (char *input_file) {
	FILE *fptr;
	char ch;
	char *ip = "";

	fptr = fopen(input_file, "r");
	if (fptr == NULL) {
		fprintf(stderr, "Failed to open file.\n");
		exit(2);
	}

	while((ch = fgetc(fptr)) != EOF) {
		if (ch >=48 && ch <= 57 || ch == 46) {
			ip = append_char(ip, ch);
			printf("%s", ip);
		}
	}

	free(ip);

	fclose(fptr);
}
