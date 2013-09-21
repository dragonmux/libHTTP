#ifndef __libHTTP_H__
#define __libHTTP_H__

#ifdef _WINDOWS
#include <winsock2.h>
#else
#endif
#include <string.h>

#ifdef _WINDOWS
	#ifdef __libHTTP_Intern__
		#define HTTP_API __declspec(dllexport)
	#else
		#ifdef __cplusplus
			#define	HTTP_API extern "C"
		#else
			#define HTTP_API __declspec(dllimport)
		#endif
	#endif
#else
#ifdef __cplusplus
	#define HTTP_API extern "C"
#else
	#define HTTP_API extern
#endif
#endif

#ifndef _WINDOWS
	#ifndef BOOL
		#include <inttypes.h>
		#define BOOL int32_t
		#define FALSE 0
		#define TRUE 1
	#endif

	// Unix sockets
	#include <sys/socket.h>
	#ifndef SOCKET
		typedef int32_t SOCKET;
	#endif
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#ifndef SOCKET_ERROR
		#define SOCKET_ERROR -1
	#endif
	#ifndef INVALID_SOCKET
		#define INVALID_SOCKET -1
	#endif
#endif

#ifndef __libHTTP__
typedef struct _Address Address;
#else
#include "internals.h"
#endif

typedef struct _Response
{
	int HTTPMajorVer, HTTPMinorVer, ResponceCode;
	char *ResponceText;
	char **Headers;
	int nHeaders;
	char *Content;
	int ContentLen;
	void *Private;
	BOOL TransferEncodingFound;
} Response;

typedef enum _httpMethod
{
	Get = 0,
	Head,
	Post,
	Options,
	Put,
	Delete,
	Trace,
	Connect
} httpMethod;

#define httpNone 0
#define httpAlreadyInitialised -1
#define httpNotInitialised -2
#define httpBadURL -3
#define httpBadProto -4
#define httpDNSError -5
#define httpNULLAddress -6
#define httpSocketError -7
#define httpConnectError -8
#define httpBadProtoResponse -9
#define httpBadProtoHeaderResponse -10
#define httpNULLResponse -11
#define httpRecvError -12
#define httpConnClosedError -13

// From init.c
HTTP_API int httpInit();
HTTP_API int httpDeinit();

// From error.c
HTTP_API int httpGetError();
HTTP_API char *httpGetErrorText();

// From connection.c
HTTP_API Address *httpLocate(char *URL);
HTTP_API void httpConnect(Address *URL);
HTTP_API void httpDisconnect(Address *URL);
HTTP_API void httpFreeAddress(Address **URL);

// From method.c
HTTP_API void httpSetMethod(Address *URL, httpMethod Method);
HTTP_API char *httpGetMethod(Address *URL);

// From request.c
HTTP_API void httpRequest(Address *URL);

// From response.c
HTTP_API Response *httpResponse(Address *URL);
HTTP_API void httpFreeResponse(Response **resp);

// From content.c
HTTP_API void httpGetContentAsync(Address *URL, Response *resp);
HTTP_API void httpWaitForContent(Response *resp);

#endif /* __libHTTP_H__ */
