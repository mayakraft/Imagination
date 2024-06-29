#pragma once

#include <string.h>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

// make sure to free the returned value after you are done using it
// this will set the length of the buffer to the second parameter,
// second parameter can be null if you don't need to know the size
char *readFile(const char *filename, long *bufferLength);

char* joinPath(const char* path, const char* filename);

// char directory_separator[] = "/";
// #ifdef WIN32
// directory_separator[0] = '\\';
// #endif

#ifdef __APPLE__
char* getMacBundleResourcesPath();
#endif

#ifdef __cplusplus
}
#endif
