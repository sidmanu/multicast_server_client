#include "common_hdr.h"

void 
util_get_int_list_from_csv(char *csv, int **pp_arr,
		int *len);


int get_next_chunk(char *input_file_name, int offset,
            int *chunk_size,
            char **p_string_chunk);

