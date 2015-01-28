#include <string.h>

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

int get_next_chunk(char *input_file_name, int offset,
            int *chunk_size,
            char *p_string_chunk)
{

	FILE *f;
    int DELTA = 10;
    char buf[MAXCHUNKSIZE];
    int cur_len;

    f = fopen(input_file_name, "r");
    fseek(f, offset, SEEK_SET);
    cur_len = fread(buf, 1, MAXCHUNKSIZE, f);
    if (cur_len < MAXCHUNKSIZE)
        goto non_clean;

    //trim from the fag end till we reach a comma
    while (cur_len > 0) {
        cur_len--;
        if (buf[cur_len - 1] == ',')
            break;
    }

    *chunk_size = cur_len;
    printf("string length : %d", *chunk_size);
    buf[cur_len] = '\0';
    strcpy(p_string_chunk, buf);

    fclose(f);
    return 0;

non_clean:
    printf("\n\n#### NON CLEAN #### \n");
    fclose(f);
    return -1;
}

