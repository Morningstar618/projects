#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "lib/ip-funcs.h"

int main(int argc, char *argv[]) {
	char *input_file;
	char *output_file;

	if (argc == 1) {
		fprintf(stderr, "Usage: %s -i <input_file> -o <output_file>");
		exit(1);
	}

	for (int i=1; i<argc; i+=2) {
		if (strcmp(argv[i], "-i") == 0)
			input_file = argv[i+1];
		else if (strcmp(argv[i], "-o") == 0)
			output_file = argv[i+1];
	}

	parse_ipv4(input_file);

	return 0;	
}

