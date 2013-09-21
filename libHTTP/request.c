#include "libHTTP.h"
#include "internals.h"

#define REQ_STR "%s %s HTTP/1.1\r\nUser-Agent: Future Consiousness/0.1 () Future Explorer 0.0.1\r\nHost: %s:%u\r\n\r\n"

void httpRequest(Address *URL)
{
	char *request, *method;
	int reqLen;

	if (Initialised == FALSE)
	{
		httpError = httpNotInitialised;
		return;
	}
	if (URL == NULL)
	{
		httpError = httpNULLAddress;
		return;
	}

	method = httpGetMethod(URL);
	reqLen = snprintf(NULL, 0, REQ_STR, method, URL->ItemLocation, URL->ServerName, URL->Port) + 1;
	request = (char *)malloc(reqLen);
	sprintf(request, REQ_STR, method, URL->ItemLocation, URL->ServerName, URL->Port);
	send(URL->Socket, request, reqLen - 1, 0);
	free(request);
}