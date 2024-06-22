#include "fs.h"

#ifdef __APPLE__

// malloc resourcePath to be larger than whatever we will put inside of it
void getMacBundleResourcesPath(char *resourcePath) {
	// This makes relative paths work in C++ in Xcode by changing
	// directory to the Resources folder inside the .app bundle
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
	char path[4096];
	if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, 4096)) {
		fputs("CFURLGetFileSystemRepresentation error", stderr);
	}
	CFRelease(resourcesURL);
	size_t len = strlen(path);
	memcpy(resourcePath, path, len);
}

// void macBundlePath() {
// 	CFBundleRef mainBundle = CFBundleGetMainBundle();
// 	CFURLRef resourcesURL = CFBundleCopyBundleURL(mainBundle);
// 	CFStringRef str = CFURLCopyFileSystemPath( resourcesURL, kCFURLPOSIXPathStyle );
// 	CFRelease(resourcesURL);
// 	char path[PATH_MAX];
// 	CFStringGetCString( str, path, FILENAME_MAX, kCFStringEncodingASCII );
// 	CFRelease(str);
// 	printf(path);
// }

// char* macBundle() {
// 	char path[PATH_MAX];
// 	// char* path = (char*)malloc(sizeof(char) * PATH_MAX);
// 	uint32_t pathLen = sizeof(path);
// 	int err = _NSGetExecutablePath(path, &pathLen);
// 	assert(!err);
// 	// return path;
// 	// printf("%s\n", path);
// }

#endif
