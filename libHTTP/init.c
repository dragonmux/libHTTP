#ifdef _WINDOWS
#include <winsock2.h>
#endif
#include "libHTTP.h"
#include "internals.h"

BOOL Initialised = FALSE;
int httpError = 0;

/** Initialises the library
 * @return error state of the call, 0 for success, -1 for already initialised, and 1000+ for a Windows Sockets Error
*/
int httpInit()
{
#ifdef _WINDOWS
	WSADATA wsaData;
	int WSAResult;
#endif

	if (Initialised == TRUE)
		return httpAlreadyInitialised;

#ifdef _WINDOWS
	WSAResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (WSAResult != NO_ERROR)
		return WSAResult;
#endif

	Initialised = TRUE;
	return httpNone;
}

int httpDeinit()
{
	if (Initialised == FALSE)
		return httpNotInitialised;

#ifdef _WINDOWS
	WSACleanup();
#endif

	Initialised = FALSE;
	return httpNone;
}
