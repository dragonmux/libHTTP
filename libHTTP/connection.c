#include "libHTTP.h"
#include "strutils.h"
#include "internals.h"

Address *httpLocate(char *URL)
{
	Address *ret;
	char *Proto = NULL, *Server = NULL, *Item = NULL, *Addr = URL;
	int i;
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
	memset(ret, 0x00, sizeof(Address));
	i = poschr(Addr, ':');
	if (i <= 0)
		goto URLError;
	Proto = (char *)malloc(i);
	memset(Proto, 0x00, i);
	memcpy(Proto, Addr, i - 1);
	Addr += i;
	if (Addr[0] != '/')
		goto URLError;
	Addr++;
	if (Addr[0] != '/')
		goto URLError;
	Addr++;
	i = poschr(Addr, '/');
	if (i == -1)
		i = strlen(Addr) + 1;
	if (i <= 1)
		goto URLError;
	Server = (char *)malloc(i);
	memcpy(Server, Addr, i - 1);
	Server[i - 1] = 0;
	Addr += i - 1;
	i = strlen(Addr);
	if (i == 0)
	{
		Item = (char *)malloc(2);
		memcpy(Item, "/", 2);
	}
	else
	{
		Item = (char *)malloc(strlen(Addr) + 1);
		strcpy(Item, Addr);
	}
	ret->ItemLocation = Item;
	if (strncasecmp(Proto, "http", 4) != 0)
		goto ProtoError;
	else if (Proto[4] == 's')
		ret->Secure = TRUE;
	else
		ret->Secure = FALSE;
	ret->Protocol = Proto;
	i = poschr(Server, ':');
	if (i == -1)
		ret->Port = (ret->Secure == TRUE ? 443 : 80);
	else
	{
		char *tmp = (char *)malloc(i);
		if (Server[i] == 0)
			ret->Port = (ret->Secure == TRUE ? 443 : 80);
		else
			ret->Port = (short)atoi(Server + i);
		memcpy(tmp, Server, i);
		tmp[i - 1] = 0;
		free(Server);
		Server = tmp;
	}
	ret->ServerName = Server;

	{
		struct hostent *host = gethostbyname(Server);
		ret->Server = (struct hostent *)malloc(sizeof(struct hostent));
		memcpy(ret->Server, host, sizeof(struct hostent));
	}
	if (ret->Server == NULL)
		goto DNSError;
	httpError = httpNone;
	return ret;

URLError:
	httpError = httpBadURL;
	goto Error;

ProtoError:
	httpError = httpBadProto;
	goto Error;

DNSError:
	httpError = httpDNSError;
	goto Error;

Error:
	free(ret->Server);
	free(ret->ServerName);
	free(ret->ItemLocation);
	free(ret->Protocol);
	free(ret);
	return NULL;
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
