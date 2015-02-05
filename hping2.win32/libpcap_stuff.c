/* 
 * $smu-mark$ 
 * $name: libpcap_stuff.c$ 
 * $author: Salvatore Sanfilippo <antirez@invece.org>$ 
 * $copyright: Copyright (C) 1999 by Salvatore Sanfilippo$ 
 * $license: This software is under GPL version 2 of license$ 
 * $date: Fri Nov  5 11:55:48 MET 1999$ 
 * $rev: 8$ 
 */
 
/*
 * Revised for Windows: Rob Turpin <rgturpin@epop3.com> 
 *                      7/03/2004          
 */ 

#include "hping2.h"

/* This is not be compiled if the target is linux without libpcap */
#if (!defined OSTYPE_LINUX) || (defined FORCE_LIBPCAP)
#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>

#ifndef WIN32
#include <sys/ioctl.h>
#include <net/bpf.h>
#else
#include <ws2tcpip.h.>
#endif

#include "globals.h"

int open_pcap()
{
	int on;

	on = 1; /* no warning if BIOCIMMEDIATE will not be compiled */
	if (opt_debug)
		printf("DEBUG: pcap_open_live(%s, 99999, 0, 1, %p)\n",
			ifname, errbuf);

	pcapfp = pcap_open_live(ifname, 99999, 0, 1, errbuf);
	if (pcapfp == NULL) {
		printf("[open_pcap] pcap_open_live: %s\n", errbuf);
		return -1;
	}
#if (!defined OSTYPE_LINUX) && (!defined __sun__) && (!defined WIN32)
	/* Return the packets to userspace as fast as possible */
	if (ioctl(pcap_fileno(pcapfp), BIOCIMMEDIATE, &on) == -1)
		perror("[open_pcap] ioctl(... BIOCIMMEDIATE ...)");
#endif
	return 0;
}

int close_pcap()
{
	pcap_close(pcapfp);
	return 0;
}

int pcap_recv(char *packet, unsigned int size)
{
        char *p = NULL;
        int pcapsize;

	if (opt_debug)
		printf("DEBUG: under pcap_recv()\n");

        while(p == NULL) {
                p = (unsigned char*) pcap_next(pcapfp, &hdr);
		if (p == NULL && opt_debug)
			printf("DEBUG: [pcap_recv] p = NULL\n");
	}

        pcapsize = hdr.caplen;

        if (pcapsize < size)
                size = pcapsize;

        memcpy(packet, p, pcapsize);

        return pcapsize;
}
#endif /* ! OSTYPE_LINUX || FORCE_LIBPCAP */
