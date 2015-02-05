/* 
 * $smu-mark$ 
 * $name: gethostname.c$ 
 * $author: Salvatore Sanfilippo <antirez@invece.org>$ 
 * $copyright: Copyright (C) 1999 by Salvatore Sanfilippo$ 
 * $license: This software is under GPL version 2 of license$ 
 * $date: Fri Nov  5 11:55:47 MET 1999$ 
 * $rev: 8$ 
 *
 * Revised for Windows: Rob Turpin <rgturpin@epop3.com> 
 *                      7/03/2004                     
 *
 */ 

#include <stdio.h>
#include <string.h>

#ifndef WIN32
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

size_t strlcpy(char *dst, const char *src, size_t siz);

char *get_hostname(char* addr)
{
	static char answer[1024];
	static char lastreq[1024] = {'\0'};	/* last request */
	struct hostent *he;
	struct in_addr naddr;
	static char *last_answerp = NULL;

	printf(" get hostname..."); fflush(stdout);
	printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
		"               "
		"\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");

	if (!strcmp(addr, lastreq))
		return last_answerp;

	strlcpy(lastreq, addr, 1024);
#ifndef WIN32
  inet_aton(addr, &naddr);
  he = gethostbyaddr((char*)&naddr, 4, AF_INET);
#else
	naddr.S_un.S_addr = inet_addr(addr);
  he = gethostbyaddr((char *)&naddr.S_un.S_addr, 4, AF_INET);
#endif

	if (he == NULL) {
		last_answerp = NULL;
		return NULL;
	}

	strlcpy(answer, he->h_name, 1024);
	last_answerp = answer;

	return answer;
}

