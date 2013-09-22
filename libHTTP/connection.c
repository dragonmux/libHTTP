#include "libHTTP.h"
#include "strutils.h"
#include "internals.h"

typedef struct _LocateInternal
{
	char *Proto;
	char *Server;
	char *Item;
} LocateInternal;

BOOL URLError()
{
	httpError = httpBadURL;
	return FALSE;
}

BOOL ConnProtoError()
{
	httpError = httpBadProto;
	return FALSE;
}

BOOL DNSError()
{
	httpError = httpDNSError;
	return FALSE;
}

BOOL tryLocate(char *Addr, Address *ret, LocateInternal *parts)
{
	int i;
	i = poschr(Addr, ':');
	if (i <= 0)
		return URLError();
	parts->Proto = (char *)malloc(i);
	memset(parts->Proto, 0x00, i);
	memcpy(parts->Proto, Addr, i - 1);
	Addr += i;
	if (Addr[0] != '/')
		return URLError();
	Addr++;
	if (Addr[0] != '/')
		return URLError();
	Addr++;
	i = poschr(Addr, '/');
	if (i == -1)
		i = strlen(Addr) + 1;
	if (i <= 1)
		return URLError();
	parts->Server = (char *)malloc(i);
	memcpy(parts->Server, Addr, i - 1);
	parts->Server[i - 1] = 0;
	Addr += i - 1;
	i = strlen(Addr);
	if (i == 0)
	{
		parts->Item = (char *)malloc(2);
		memcpy(parts->Item, "/", 2);
	}
	else
	{
		parts->Item = (char *)malloc(strlen(Addr) + 1);
		strcpy(parts->Item, Addr);
	}
	ret->ItemLocation = parts->Item;
	parts->Item = NULL;
	if (strncasecmp(parts->Proto, "http", 4) != 0)
		return ConnProtoError();
	else if (parts->Proto[4] == 's')
		ret->Secure = TRUE;
	else
		ret->Secure = FALSE;
	ret->Protocol = parts->Proto;
	parts->Proto = NULL;
	i = poschr(parts->Server, ':');
	if (i == -1)
		ret->Port = (ret->Secure == TRUE ? 443 : 80);
	else
	{
		char *tmp = (char *)malloc(i);
		if (parts->Server[i] == 0)
			ret->Port = (ret->Secure == TRUE ? 443 : 80);
		else
			ret->Port = (short)atoi(parts->Server + i);
		memcpy(tmp, parts->Server, i);
		tmp[i - 1] = 0;
		free(parts->Server);
		parts->Server = tmp;
	}
	ret->ServerName = parts->Server;
	parts->Server = NULL;

	ret->Server = (struct hostent *)malloc(sizeof(struct hostent));
	if (ret->Server == NULL)
		return DNSError();
	{
		struct hostent *host = gethostbyname(ret->ServerName);
		if (host == NULL)
			return DNSError();
		memcpy(ret->Server, host, sizeof(struct hostent));
	}

	httpError = httpNone;
	return TRUE;
}

Address *httpLocate(char *URL)
{
	Address *ret;
	LocateInternal parts;
	if (URL == NULL)
	{
		httpError = httpBadURL;
		return NULL;
	}
	if (Initialised == FALSE)
	{
		httpError = httpNotInitialised;
		return NULL;
	}

	ret = (Address *)malloc(sizeof(Address));
	if (ret == NULL)
	{
		httpError = httpOutOfMem;
		return NULL;
	}
	memset(ret, 0, sizeof(Address));
	memset(&parts, 0, sizeof(LocateInternal));
	if (!tryLocate(URL, ret, &parts))
	{
		free(parts.Proto);
		free(parts.Server);
		free(parts.Item);
		free(ret->Server);
		free(ret->ServerName);
		free(ret->ItemLocation);
		free(ret->Protocol);
		free(ret);
		return NULL;
	}
	return ret;
}

void httpConnect(Address *URL)
{
	struct sockaddr_in service;
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
	if (URL->Server == NULL)
	{
		httpError = httpDNSError;
		return;
	}

	URL->Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (URL->Socket == INVALID_SOCKET)
	{
		httpError = httpSocketError;
		return;
	}

	URL->ServerIP = inet_addr(inet_ntoa(*(struct in_addr *) *(URL->Server->h_addr_list)));
	memset(&service, 0x00, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = URL->ServerIP;
	service.sin_port = htons(URL->Port);
	if (connect(URL->Socket, (__sockaddr *)&service, sizeof(service)) == SOCKET_ERROR)
	{
		httpError = httpConnectError;
		return;
	}
	URL->Connected = TRUE;

	return;
}

void httpDisconnect(Address *URL)
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

	if (URL->Socket != INVALID_SOCKET)
	{
		if (URL->Connected == TRUE)
#ifdef _WINDOWS
			shutdown(URL->Socket, FD_CLOSE);
		closesocket(URL->Socket);
#else
			shutdown(URL->Socket, SHUT_RDWR);
		close(URL->Socket);
#endif
	}
	return;
}

void httpFreeAddress(Address **URL)
{
	Address *a;
	if (Initialised == FALSE)
	{
		httpError = httpNotInitialised;
		return;
	}
	if (URL == NULL || *URL == NULL)
	{
		httpError = httpNULLAddress;
		return;
	}
	a = *URL;

	free(a->ServerName);
	free(a->ItemLocation);
	free(a->Protocol);
	free(a->Server);
	free(a);
	*URL = NULL;
}
