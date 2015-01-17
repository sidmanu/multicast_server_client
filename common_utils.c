#include "common_hdr.h"

void 
util_get_int_list_from_csv(char *csv, int **pp_arr,
		int *len)
{
	char *p = csv;
	int count = 0;
	int idx;

	int *arr;

	while (1) {
		count++;

		p = strchr(p, ','); // next comma
		if (p == NULL)
			break;  /* no more commas, end of string */
		else
			p++;    /* skip over the comma */
	}


	*len = count;
	*pp_arr = malloc(sizeof(int) * count);

	arr = *pp_arr;

	p = csv;
	idx = 0;
	while (1) {
		int i = atoi(p);
		arr[idx] = i;

		p = strchr(p, ','); // next comma
		if (p == NULL)
			break;  
		p++;    /* skip over the comma */
		idx++;
	}
}


