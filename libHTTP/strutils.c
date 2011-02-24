#include <ctype.h>
#include "strutils.h"

int poschr(const char *haystack, const char needle)
{
	size_t i = 0, len = strlen(haystack);
	while (haystack[i] != needle && len != i)
		i++;
	if (haystack[i] == needle)
		return i + 1; //found the char @ pos i + 1
	else
		return -1; // didn't find the character
}