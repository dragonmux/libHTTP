#include <ctype.h>
#include <string.h>

int strcasecmp(const char *s1, const char *s2)
{
	while (tolower(*(unsigned char *)s1) == tolower(*(unsigned char *)s2))
	{
		if (*s1 == '\0' || *s2 == '\0')
			return 0;
		s1++;
		s2++;
	}
	return tolower(*(unsigned char *)s1) - tolower(*(unsigned char *)s2);
}

int strncasecmp(const char *s1, const char *s2, unsigned int n)
{
	if (n == 0)
	return 0;

	while ((n-- != 0) && (tolower(*(unsigned char *) s1) == tolower(*(unsigned char *) s2)))
	{
		if (n == 0 || *s1 == '\0' || *s2 == '\0')
			return 0;
		s1++;
		s2++;
	}

	return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}

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