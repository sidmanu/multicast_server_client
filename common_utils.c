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
    char buf[MAXCHUNKSIZE + 1] = {'\0'};
    int cur_len;
    int ret = 0;
    int index;
    f = fopen(input_file_name, "r");
    fseek(f, offset, SEEK_SET);
    cur_len = fread(buf, sizeof(char), MAXCHUNKSIZE, f);

    //trim from the fag end till we reach a comma
    //for a properly terminated number
    if(buf == NULL || buf[0] == '\0')
        return -1;
    buf[cur_len] = '\0';
    index = cur_len - 1;
    while (index > 0 && (cur_len + 1) == MAXCHUNKSIZE) {
        if (buf[index] == ',')
            break;
        index--;
    }
    if(buf[index] == '\n') {
        ret = -1;
    }
    *chunk_size = cur_len = index;
    strncpy(p_string_chunk, buf,cur_len);

    fclose(f);
    return ret;
}

