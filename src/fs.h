#include <unistd.h> // for chdir
#include <libgen.h> // for dirname
#include <mach-o/dyld.h> // for _NSGetExecutablePath
#include <limits.h> // for PATH_MAX?

//#include <CoreFoundation/CoreFoundation.h>

void macBundlePath() {
//    CFBundleRef mainBundle = CFBundleGetMainBundle();
//    CFURLRef resourcesURL = CFBundleCopyBundleURL(mainBundle);
//    CFStringRef str = CFURLCopyFileSystemPath( resourcesURL, kCFURLPOSIXPathStyle );
//    CFRelease(resourcesURL);
//    char path[PATH_MAX];
//    CFStringGetCString( str, path, FILENAME_MAX, kCFStringEncodingASCII );
//    CFRelease(str);
//    printf(path);
}

char* macBundle() {
	char path[PATH_MAX];
	// char* path = (char*)malloc(sizeof(char) * PATH_MAX);
	uint32_t pathLen = sizeof(path);
	int err = _NSGetExecutablePath(path, &pathLen);
	assert(!err);
	// return path;
	// printf("%s\n", path);
}
