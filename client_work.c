#include <stdio.h>
#include <string.h>

#include "common_hdr.h"
#include "client.h"
#include "client_work.h"

long 
do_work_sum_csv(char *csv_str)
{
	size_t field_len;
 	const char *s = csv_str;
	const char delims[] = ",";	
	char num_string[10];
	long sum = 0;
	int num;

    do {
        field_len = strcspn(s, delims);

		strncpy(num_string, s, field_len);
		num_string[field_len] = '\0';
		num = atoi(num_string);
		sum += num;
		debug_print_2("\nnum_string: %s, num: %d", num_string, num);
        debug_print_3("\"%.*s\"\n", (int)field_len, s);
        s += field_len;
    } while (*s++);

	return sum;
}	
