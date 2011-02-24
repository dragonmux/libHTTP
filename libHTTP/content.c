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

void *httpContentThread(void *p_resp)
{
	Address *URL;
	int i;
	Response *resp = (Response *)p_resp;
	ContentInternals *Intern = (ContentInternals *)resp->Private;
	URL = Intern->URL;
	pthread_mutex_lock(&Intern->mutex);

	if (resp->TransferEncodingFound == TRUE)
	{
		int nChunks = 0, len, *lenChunks = (int *)malloc(0), i;
		char **Chunks = (char **)malloc(0);
		free(resp->Content);
		do
		{
			char *HexLen, r[2];
			int lenHexLen = 0;
			HexLen = (char *)malloc(1);
			HexLen[0] = 0;
			do
			{
				recv(URL->Socket, r, 1, 0);
				if (r[0] != '\r')
				{
					HexLen = (char *)realloc(HexLen, lenHexLen + 2);
					HexLen[lenHexLen++] = r[0];
					HexLen[lenHexLen] = 0;
				}
			}
			while (r[0] != '\r');
			recv(URL->Socket, &r[1], 1, 0);
			if (strncmp(r, "\r\n", 2) != 0)
				goto HeaderProtoError;
			len = HexToInt(HexLen);
			if (len != 0)
			{
				int i;
				Chunks = (char **)realloc(Chunks, sizeof(char *) * (nChunks + 1));
				lenChunks = (int *)realloc(lenChunks, sizeof(int) * (nChunks + 1));
				Chunks[nChunks] = (char *)malloc(len);
				lenChunks[nChunks] = len;
				for (i = 0; i < len; )
					i += recv(URL->Socket, Chunks[nChunks] + i, len - i, 0);
				nChunks++;
			}
			recv(URL->Socket, r, 2, 0);
			if (strncmp(r, "\r\n", 2) != 0)
				goto HeaderProtoError;
		}
		while (len != 0);
		resp->ContentLen = 1;
		resp->Content = (char *)malloc(1);
		for (i = 0; i < nChunks; i++)
		{
			int len = lenChunks[i];
			resp->Content = (char *)realloc(resp->Content, resp->ContentLen + len);
			memcpy(resp->Content + resp->ContentLen - 1, Chunks[i], len);
			resp->ContentLen += len;
			free(Chunks[i]);
		}
		resp->Content[resp->ContentLen - 1] = 0;
		free(Chunks);
		free(lenChunks);
		pthread_mutex_unlock(&Intern->mutex);
		return 0;

HeaderProtoError:
		for (i = 0; i < nChunks; i++)
			free(Chunks[i]);
		free(Chunks);
		free(lenChunks);
		httpError = httpBadProtoHeaderResponse;
		goto Error;
	}
	else
	{
		for (i = 0; i < resp->ContentLen; )
			i += recv(URL->Socket, resp->Content + i, resp->ContentLen - i, 0);
	}

	pthread_mutex_unlock(&Intern->mutex);
	return 0;

Error:
	pthread_mutex_unlock(&Intern->mutex);
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
