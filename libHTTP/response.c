/*
 * This file is part of libHTTP
 * Copyright © 2013 Rachel Mant (dx-mon@users.sourceforge.net)
 *
 * libHTTP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libHTTP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "libHTTP.h"
#include "internals.h"
#include "strutils.h"

BOOL IsDigit(char x)
{
	x -= '0';
	if (x >= 0 && x <= 9)
		return TRUE;
	return FALSE;
}

void GetNextDigit(int *dest, char *src)
{
	if (IsDigit(src[0]) == TRUE)
	{
		*dest *= 10;
		*dest += src[0] - '0';
	}
}

BOOL ProtoError()
{
	httpError = httpBadProtoResponse;
	return FALSE;
}

BOOL HeaderProtoError()
{
	httpError = httpBadProtoHeaderResponse;
	return FALSE;
}

BOOL NewlineCheck(const char *buff)
{
	if (memcmp(buff , "\r\n", 2) != 0)
		return HeaderProtoError();
	return TRUE;
}

BOOL tryReadHeaders(Address *URL, Response *ret)
{
	uint32_t recved;
	int rtl;
	static char buff[5] = {0};

	if (!tryRecv(URL->Socket, buff, 5, &recved) || recved != 5)
		return FALSE;
	if (strncmp(buff, "HTTP/", 5) != 0)
		return ProtoError();
	do
	{
		if (!tryRecv(URL->Socket, buff, 1, &recved) || recved != 1)
			return FALSE;
		GetNextDigit(&ret->HTTPMajorVer, buff);
	}
	while (IsDigit(buff[0]) == TRUE);
	if (buff[0] != '.')
		return ProtoError();
	do
	{
		if (!tryRecv(URL->Socket, buff, 1, &recved) || recved != 1)
			return FALSE;
		GetNextDigit(&ret->HTTPMinorVer, buff);
	}
	while (IsDigit(buff[0]) == TRUE);
	if (buff[0] != ' ')
		return ProtoError();
	if (!tryRecv(URL->Socket, buff, 3, &recved) || recved != 3)
		return FALSE;
	buff[3] = 0;
	ret->ResponceCode = atoi(buff);
	if (!tryRecv(URL->Socket, buff, 1, &recved) || recved != 1)
		return FALSE;
	ret->ResponceText = (char *)malloc(1);
	ret->ResponceText[0] = 0;
	rtl = 0;
	do
	{
		if (!tryRecv(URL->Socket, buff, 1, &recved) || recved != 1)
			return FALSE;
		if (buff[0] != '\r' && buff[0] != '\n')
		{
			ret->ResponceText = (char *)realloc(ret->ResponceText, rtl + 2);
			ret->ResponceText[rtl] = buff[0];
			rtl++;
		}
	}
	while (buff[0] != '\r' && buff[0] != '\n');
	ret->ResponceText[rtl + 1] = 0;
	if (!tryRecv(URL->Socket, buff + 1, 1, &recved) || recved != 1)
		return FALSE;
	if (!NewlineCheck(buff))
		return FALSE;
	ret->Headers = malloc(0);
	while (1)
	{
		char *Header = malloc(1);
		int lenHeader = 0;
		Header[0] = 0;
		do
		{
			if (!tryRecv(URL->Socket, buff, 1, &recved) || recved != 1)
				return FALSE;
			if (buff[0] != '\r' && buff[0] != '\n')
			{
				Header = realloc(Header, lenHeader + 2);
				Header[lenHeader] = buff[0];
				lenHeader++;
			}
		}
		while (buff[0] != ':' && buff[0] != '\r' && buff[0] != '\n');
		Header[lenHeader + 1] = 0;
		if (buff[0] == '\r' || buff[0] == '\n' && lenHeader == 0)
		{
			if (buff[0] == '\n')
			{
				free(Header);
				return HeaderProtoError();
			}
			if (!tryRecv(URL->Socket, buff + 1, 1, &recved) || recved != 1)
				return FALSE;
			if (!NewlineCheck(buff))
				return FALSE;
			break;
		}
		else
		{
			if (strcasecmp(Header, "Content-Length:") == 0)
			{
				if (!tryRecv(URL->Socket, buff, 1, &recved) || recved != 1)
					return FALSE;
				Header = realloc(Header, lenHeader + 2);
				Header[lenHeader] = buff[0];
				lenHeader++;
				do
				{
					if (!tryRecv(URL->Socket, buff, 1, &recved) || recved != 1)
						return FALSE;
					if (IsDigit(buff[0]) == TRUE)
					{
						Header = (char *)realloc(Header, lenHeader + 2);
						Header[lenHeader] = buff[0];
						lenHeader++;
					}
					GetNextDigit(&ret->ContentLen, buff);
				}
				while (IsDigit(buff[0]) == TRUE);
				Header[lenHeader + 1] = 0;
				if (ret->Content != NULL)
					free(ret->Content);
				ret->Content = (char *)malloc(ret->ContentLen + 1);
				memset(ret->Content, 0x00, ret->ContentLen + 1);
			}
			else
			{
				if (strcasecmp(Header, "Transfer-Encoding:") == 0)
					ret->TransferEncodingFound = TRUE;
				do
				{
					if (!tryRecv(URL->Socket, buff, 1, &recved) || recved != 1)
						return FALSE;
					if (buff[0] != '\r' && buff[0] != '\n')
					{
						Header = (char *)realloc(Header, lenHeader + 2);
						Header[lenHeader] = buff[0];
						lenHeader++;
					}
				}
				while (buff[0] != '\r' && buff[0] != '\n');
				Header[lenHeader + 1] = 0;
			}
			ret->Headers = (char **)realloc(ret->Headers, sizeof(char *) * (ret->nHeaders + 1));
			ret->Headers[ret->nHeaders] = Header;
			ret->nHeaders++;
			if (!tryRecv(URL->Socket, buff + 1, 1, &recved) || recved != 1)
				return FALSE;
			if (!NewlineCheck(buff))
				return FALSE;
		}
	}
	httpGetContentAsync(URL, ret);
	//recv(URL->Socket, ret->Content, ret->ContentLen, 0);
	return TRUE;
}

Response *httpResponse(Address *URL)
{
	Response *ret;

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

	ret = (Response *)malloc(sizeof(Response));
	memset(ret, 0x00, sizeof(Response));
	if (tryReadHeaders(URL, ret) == FALSE)
	{
		httpFreeResponse(&ret);
		return NULL;
	}
	return ret;
}

void httpFreeResponse(Response **resp)
{
	Response *r;
	int i;
	if (Initialised == FALSE)
	{
		httpError = httpNotInitialised;
		return;
	}
	if (resp == NULL || *resp == NULL)
	{
		httpError = httpNULLResponse;
		return;
	}
	r = *resp;

	free(r->ResponceText);
	for (i = 0; i < r->nHeaders; i++)
		free(r->Headers[i]);
	free(r->Headers);
	free(r->Content);
	free(r);
	*resp = NULL;
}
