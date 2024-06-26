// #include "general.h"
// #include <stdlib.h>
// #include <string.h>
// #include <stdio.h>

// char* concat_path_filename(const char* path, const char* filename) {
// 	// Calculate the total length needed, including the null terminator
// 	size_t path_len = strlen(path);
// 	size_t filename_len = strlen(filename);
// 	size_t total_len = path_len + filename_len + 1; // +1 for the null terminator

// 	// Allocate memory for the result
// 	char* result = (char*)malloc(total_len * sizeof(char));
// 	if (result == NULL) {
// 		// Handle memory allocation failure
// 		fputs("Memory allocation failed\n", stderr);
// 		return NULL;
// 	}

// 	// Copy the path to the result
// 	strcpy(result, path);
// 	// Concatenate the filename to the result
// 	strcat(result, filename);

// 	return result;
// }
