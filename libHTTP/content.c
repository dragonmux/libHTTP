#include "libHTTP.h"
#include "internals.h"
#include "strutils.h"
#include "pthread.h"

typedef struct _ContentInternals
{
	pthread_mutex_t mutex;
	pthread_t thread;
	Address *URL;
} ContentInternals;

typedef struct _ChunkedInternals
{
	uint32_t nChunks;
	uint32_t *lenChunks;
	char **Chunks;
} ChunkedInternals;

int HexToInt(char *Hex)
{
	int i, len = strlen(Hex), ret = 0;
	for (i = 0; i < len; i++)
	{
		int t = toupper(Hex[i]) - 0x30; // '0' == 0x30
		if (t >= 10)
		{
			t -= 0x07; // 'A' == 0x41, 0x41 - 0x30 = 0x11, 0x11 - 0x07 = 0x0A;
			if (t > 16 || t < 10)
				return ret;
		}
		ret *= 16;
		ret += t;
	}
	return ret;
}

void freeChunks(ChunkedInternals *chunks)
{
	uint32_t i;
	for (i = 0; i < chunks->nChunks; i++)
		free(chunks->Chunks[i]);
	free(chunks->Chunks);
	free(chunks->lenChunks);
	free(chunks);
}

BOOL HeaderResponseProtoError(ChunkedInternals *chunks)
{
	freeChunks(chunks);
	httpError = httpBadProtoHeaderResponse;
	return FALSE;
}

BOOL tryRecvChunkedContent(Response *resp, Address *URL)
{
	uint32_t len, i, read;
	ChunkedInternals *chunks = malloc(sizeof(ChunkedInternals));
	if (chunks == NULL)
		return FALSE;
	memset(chunks, 0, sizeof(ChunkedInternals));

	free(resp->Content);
	do
	{
		char *HexLen, r[2];
		int lenHexLen = 0;
		HexLen = (char *)malloc(1);
		HexLen[0] = 0;
		do
		{
			if (!tryRecv(URL->Socket, r, 1, &read) || read != 1)
				return HeaderResponseProtoError(chunks);
			if (r[0] != '\r')
			{
				HexLen = realloc(HexLen, lenHexLen + 2);
				HexLen[lenHexLen++] = r[0];
				HexLen[lenHexLen] = 0;
			}
		}
		while (r[0] != '\r');
		if (!tryRecv(URL->Socket, r + 1, 1, &read) || read != 1)
			return HeaderResponseProtoError(chunks);
		if (strncmp(r, "\r\n", 2) != 0)
			return HeaderResponseProtoError(chunks);
		len = HexToInt(HexLen);
		if (len != 0)
		{
			chunks->Chunks = realloc(chunks->Chunks, sizeof(char *) * (chunks->nChunks + 1));
			chunks->lenChunks = realloc(chunks->lenChunks, sizeof(int) * (chunks->nChunks + 1));
			chunks->Chunks[chunks->nChunks] = malloc(len);
			chunks->lenChunks[chunks->nChunks] = len;
			for (i = 0; i < len; )
			{
				if (!tryRecv(URL->Socket, chunks->Chunks[chunks->nChunks] + i, len - i, &read))
					return HeaderResponseProtoError(chunks);
				i += read;
			}
			chunks->nChunks++;
		}
		if (!tryRecv(URL->Socket, r, 2, &read) || read != 2)
			return HeaderResponseProtoError(chunks);
		if (strncmp(r, "\r\n", 2) != 0)
			return HeaderResponseProtoError(chunks);
	}
	while (len != 0);
	resp->ContentLen = 1;
	resp->Content = malloc(1);
	for (i = 0; i < chunks->nChunks; i++)
	{
		int len = chunks->lenChunks[i];
		resp->Content = realloc(resp->Content, resp->ContentLen + len);
		memcpy(resp->Content + resp->ContentLen - 1, chunks->Chunks[i], len);
		resp->ContentLen += len;
		free(chunks->Chunks[i]);
	}
	resp->Content[resp->ContentLen - 1] = 0;
	chunks->nChunks = 0;
	freeChunks(chunks);
	return TRUE;
}

BOOL tryRecvContent(Response *resp, Address *URL)
{
	if (resp->TransferEncodingFound == TRUE)
		return tryRecvChunkedContent(resp, URL);
	else
	{
		int i;
		uint32_t read;
		for (i = 0; i < resp->ContentLen; )
		{
			if (!tryRecv(URL->Socket, resp->Content + i, resp->ContentLen - i, &read))
				return FALSE;
			i += read;
		}
	}
	return TRUE;
}

void *httpContentThread(void *p_resp)
{
	BOOL res;
	Response *resp = (Response *)p_resp;
	ContentInternals *Intern = (ContentInternals *)resp->Private;

	pthread_mutex_lock(&Intern->mutex);
	res = tryRecvContent(resp,Intern->URL);
	pthread_mutex_unlock(&Intern->mutex);
	if (res)
		return 0;
	else
		return (void *)httpError;
}

void httpGetContentAsync(Address *URL, Response *resp)
{
	pthread_mutexattr_t mutex_attr;
	pthread_attr_t thread_attr;
	ContentInternals *Intern;
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
	if (resp == NULL)
	{
		httpError = httpNULLResponse;
		return;
	}

	if (resp->Private != NULL)
		free(resp->Private);
	resp->Private = malloc(sizeof(ContentInternals));
	memset(resp->Private, 0x00, sizeof(ContentInternals));
	Intern = (ContentInternals *)resp->Private;
	Intern->URL = URL;

	pthread_mutexattr_init(&mutex_attr);
	pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutex_init(&Intern->mutex, &mutex_attr);
	pthread_mutexattr_destroy(&mutex_attr);

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_PROCESS);
	pthread_create(&Intern->thread, &thread_attr, httpContentThread, (void *)resp);
	pthread_attr_destroy(&thread_attr);
#ifdef _WINDOWS
	Sleep(0);
#else
	sleep(0);
#endif
}

void httpWaitForContent(Response *resp)
{
	ContentInternals *Intern;
	if (Initialised == FALSE)
	{
		httpError = httpNotInitialised;
		return;
	}
	if (resp == NULL)
	{
		httpError = httpNULLResponse;
		return;
	}

	Intern = (ContentInternals *)resp->Private;
	pthread_mutex_lock(&Intern->mutex);
	pthread_mutex_unlock(&Intern->mutex);

	free(Intern);
	resp->Private = NULL;
}
