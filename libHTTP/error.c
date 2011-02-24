#include "libHTTP.h"
#include "internals.h"
#include <malloc.h>
#ifndef _WINDOWS
#include <errno.h>
#endif

#ifdef _WINDOWS
#define ErrorFormat "%i; Socket error: %i"
#else
#define ErrorFormat "%i; Socket error: (%i) \"%s\""
#endif

int httpGetError()
{
	int ret = httpError;
	httpError = 0;
	return ret;
}

char *httpGetErrorText()
{
#ifdef _WINDOWS
	int wsaerr;
#else
	const char *perr;
#endif
	int err = httpGetError(), retLen;
	char *ret;
	if (err == 0)
		return "0; No Error.";
#ifdef _WINDOWS
	wsaerr = WSAGetLastError();
	retLen = snprintf(NULL, 0, ErrorFormat, err, wsaerr) + 1;
	ret = (char *)malloc(retLen);
	sprintf(ret, ErrorFormat, err, wsaerr);
#else
	perr = sys_errlist[errno];
	retLen = snprintf(NULL, 0, ErrorFormat, err, errno, perr) + 1;
	ret = (char *)malloc(retLen);
	sprintf(ret, ErrorFormat, err, errno, perr);
	errno = 0;
#endif
	return ret;
}
