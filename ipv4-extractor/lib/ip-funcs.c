#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Global Vars */
char *ip = "\0";

void is_ipv4(char *ip)
{
	/* Adding '.' at the end of the *ip pointer for filtering purposes */
	int ip_len = strlen(ip);
	char *temp_ip = (char *)malloc((sizeof(char)) * ip_len);
	strcpy(temp_ip, ip);
	temp_ip[ip_len] = '.';

	/* ip_buffer for storing ip octets */
	char *ip_buffer = (char *)malloc(sizeof(char) * 3);
	int digit_count = 0;

	/* variables on whose basis a string will be determined as an ip */
	int valid_octet = 0;
	int dot_count = 0;

	for (int i = 0; temp_ip[i] != '\0'; i++)
	{
		if (temp_ip[i] != '.')
		{
			ip_buffer[digit_count] = temp_ip[i];
			digit_count++;
		}
		else // Checking for valid octets
		{
			digit_count = 0;
			dot_count++;

			int octet = atoi(ip_buffer);

			if (octet >= 0 && octet <= 255)
			{
				valid_octet++;
			}

			/* Resetting ip_buffer */
			ip_buffer[0] = '\0';
			ip_buffer[1] = '\0';
			ip_buffer[2] = '\0';
		}
	}

	if (valid_octet == 4 && dot_count == 4)
		printf("%s\n", ip);
}

void append_char(char *str, char ch)
{
	size_t len = strlen(str);
	char *temp = str;

	ip = (char *)malloc((sizeof(char)) * (len + 2));
	if (ip == NULL)
	{
		fprintf(stderr, "Failed to allocate memory to new string");
		exit(3);
	}

	strcpy(ip, temp);

	if ((ch >= 48 && ch <= 57) || ch == 46)
	{
		ip[len] = ch;
		ip[len + 1] = '\0';
	}
	else
	{
		/* Check if the appended string is a valid ip */
		is_ipv4(ip);
		ip = "\0";
	}
}

void parse_ipv4(char *input_file)
{
	FILE *fptr;
	char ch;

	fptr = fopen(input_file, "r");
	if (fptr == NULL)
	{
		fprintf(stderr, "Failed to open file.\n");
		exit(2);
	}

	while ((ch = fgetc(fptr)) != EOF)
	{
		append_char(ip, ch);
	}
}