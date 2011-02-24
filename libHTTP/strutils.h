#include <string.h>
#ifdef _WINDOWS
#define strcasecmp stricmp
#define strncasecmp strnicmp
#endif
int poschr(const char *haystack, const char needle);