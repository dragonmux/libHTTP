#include "libHTTP.h"
#include "internals.h"

BOOL tryRecv(SOCKET sock, char *buffer, uint32_t bufferLen, uint32_t *retLen)
{
	int ret = recv(sock, buffer, bufferLen, 0);
	if (ret < 0)
	{
		httpError = httpRecvError;
		return FALSE;
	}
	else if (ret == 0)
	{
		if (bufferLen == 0)
			return TRUE;
		else
		{
			// Server shut down on us
			httpError = httpConnClosedError;
			return FALSE;
		}
	}
	*retLen = (uint32_t)ret;
	return TRUE;
}
