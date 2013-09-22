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

#define REQ_STR "%s %s HTTP/1.1\r\nUser-Agent: Future Consiousness/0.1 () Future Explorer 0.0.1\r\nHost: %s:%u\r\n\r\n"

void httpRequest(Address *URL)
{
	char *request;
	const char *method;
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
	request = malloc(reqLen);
	sprintf(request, REQ_STR, method, URL->ItemLocation, URL->ServerName, URL->Port);
	send(URL->Socket, request, reqLen - 1, 0);
	free(request);
}
