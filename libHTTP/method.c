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