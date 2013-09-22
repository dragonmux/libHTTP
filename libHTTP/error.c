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
	return ret;
}

void httpResetError()
{
	httpError = 0;
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
	httpResetError();
	if (err == 0)
		return "0; No Error.";
#ifdef _WINDOWS
	wsaerr = WSAGetLastError();
	retLen = snprintf(NULL, 0, ErrorFormat, err, wsaerr) + 1;
	ret = (char *)malloc(retLen);
	sprintf(ret, ErrorFormat, err, wsaerr);
#else
	perr = strerror(errno);
	retLen = snprintf(NULL, 0, ErrorFormat, err, errno, perr) + 1;
	ret = (char *)malloc(retLen);
	sprintf(ret, ErrorFormat, err, errno, perr);
	errno = 0;
#endif
	return ret;
}
