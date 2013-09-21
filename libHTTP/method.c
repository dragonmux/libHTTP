#include "libHTTP.h"
#include "internals.h"

static char *Methods[] = 
{
	"GET",
	"HEAD",
	"POST",
	"OPTIONS",
	"PUT",
	"DELETE",
	"TRACE",
	"CONNECT"
};

void httpSetMethod(Address *URL, httpMethod Method)
{
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
	if (Method > Connect)
		return;
	URL->method = (int)Method;
}

char *httpGetMethod(Address *URL)
{
	if (Initialised == FALSE)
	{
		httpError = httpNotInitialised;
		return NULL;
	}
	if (URL == NULL)
	{
		httpError = httpNULLAddress;
		return NULL;
	}
	return Methods[URL->method];
}