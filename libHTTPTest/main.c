#include <stdio.h>
#ifdef _WINDOWS
#include <conio.h>
#else
#include <malloc.h>
extern int getch();
#endif
#include <libHTTP.h>

char a = 0;

int strcasecmp(const char *s1, const char *s2)
{
	while ((tolower(*(unsigned char *) s1) == tolower(*(unsigned char *) s2)))
	{
		if (*s1 == '\0' || *s2 == '\0')
			return 0;
		s1++;
		s2++;
	}

	return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
}

void GetChar()
{
	fread(&a, 1, 1, stdin);
}

char *getLine()
{
	char *ret = (char *)malloc(1);
	int retLen = 0;
	ret[0] = 0;
	while (1)
	{
		if (a == '\n' || a == '\r')
			return ret;
		ret = (char *)realloc(ret, retLen + 2);
		ret[retLen++] = a;
		ret[retLen] = 0;
		GetChar();
	}
	// Just to keep MSVC++ happy, never gets to run:
	return NULL;
}

void SkipWhite()
{
	while (a == '\n' || a == '\r')
		GetChar();
}

int main(int argc, char **argv)
{
	char *Line = NULL;
	printf("httpInit() returned %i\n", httpInit());

	GetChar();
	do
	{
		free(Line);
		SkipWhite();
		Line = getLine();
		if (strcasecmp(Line, "Quit") != 0)
		{
			struct in_addr srv_addr = {0};
			Response *resp;
			Address *Addr = httpLocate(Line);
			printf("httpError is %s after call to httpLocate()\n", httpGetErrorText());
			printf("httpLocate() returned 0x%08X\n", Addr);
			httpConnect(Addr);
			printf("httpError is %s after call to httpConnect()\n", httpGetErrorText());
			if (Addr != NULL)
			{
				srv_addr.s_addr = ((int *)Addr)[1];
				printf("Server IP/Port is: %s/%i\n", inet_ntoa(srv_addr), ((int *)Addr)[8]);
			}
			httpRequest(Addr);
			printf("httpError is %s after call to httpRequest()\n", httpGetErrorText());
			resp = httpResponse(Addr);
			printf("httpError is %s after call to httpResponse()\n", httpGetErrorText());
			if (resp != NULL)
			{
				int i;
				printf("HTTP/%i.%i %i %s\n", resp->HTTPMajorVer, resp->HTTPMinorVer, resp->ResponceCode, resp->ResponceText);
				printf("Headers:\n");
				for (i = 0; i < resp->nHeaders; i++)
					printf("%s\n", resp->Headers[i]);
				printf("\n[Getting content....");
				httpWaitForContent(resp);
				printf(" done!]\n");
				printf("Content:\n%s\n", resp->Content);
			}
			httpDisconnect(Addr);
			printf("httpError is %s after call to httpDisconnect()\n", httpGetErrorText());
			httpFreeResponse(&resp);
			printf("httpError is %s after call to httpFreeResponse()\n", httpGetErrorText());
			httpFreeAddress(&Addr);
			printf("httpError is %s after call to httpFreeAddress()\n", httpGetErrorText());
		}
	}
	while (strcasecmp(Line, "Quit") != 0);

	printf("httpDeinit() returned %i\n", httpDeinit());
	printf("\nPress any key to continue....\n");
	getch();
	return 0;
}
