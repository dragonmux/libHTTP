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
