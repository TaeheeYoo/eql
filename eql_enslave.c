/*
 * eql_enslave
 *
 * modeled from ifslave.c by Alan Cox (presumably?)
 *
 * (c) Copyright 1995 Simon Janes
 * NCM: Network and Communications Management, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <linux/if.h>
#ifndef list_head
struct list_head {
	struct list_head *next, *prev;
};
#endif
#include <linux/if_eql.h>

void check_running(char *device_name);

int s;

	int
main(int argc, char **argv)
{
	struct ifreq ifr;
	slaving_request_t slaving_request;
	char master_name[16];
	int slave_mtu;
	int master_mtu;

	if (argc != 4)
	{
		fprintf (stderr, "usage: %s <master> <slave> <priority>\n", argv[0]);
		exit (1);
	}

	strcpy (master_name, argv[1]);
	strcpy (slaving_request.slave_name, argv[2]);
	slaving_request.priority = atol (argv[3]);

	s = socket (AF_INET, SOCK_DGRAM, 0);
	if ( s == -1)
	{
		perror ("socket");
		exit (1);
	}

	check_running (master_name);
	check_running (slaving_request.slave_name);

	strcpy (ifr.ifr_name, slaving_request.slave_name);
	if (ioctl (s, SIOCGIFMTU, &ifr) == -1)
	{
		perror("get MTU on slave failed");
		exit (1);
	}
	slave_mtu = ifr.ifr_mtu;

	strcpy (ifr.ifr_name, master_name);
	if (ioctl (s, SIOCGIFMTU, &ifr) == -1)
	{
		perror("get MTU on master failed");
		exit (1);
	}
	master_mtu = ifr.ifr_mtu;

	if ( master_mtu != slave_mtu )
	{
		fprintf (stderr, 
				"master (%d) and slave (%d) MTU settings do not match\n",
				master_mtu, slave_mtu);
		exit (1);
	}

	strcpy (ifr.ifr_name, master_name);
	ifr.ifr_data = (caddr_t) &slaving_request;

	if(ioctl (s,EQL_ENSLAVE, &ifr)==-1)
	{
		perror("EQL_ENSLAVE failed");
		exit (1);
	}

	return 0;
}

	void
check_running(char *name)
{
	struct ifreq ifr;

	strcpy (ifr.ifr_name, name);
	if ( ioctl (s, SIOCGIFFLAGS, &ifr) == -1)
	{
		perror (name);
		exit (1);
	}
	if (ifr.ifr_flags & ((IFF_RUNNING | IFF_UP) != (IFF_RUNNING | IFF_UP)))
	{
		fprintf (stderr, "Device '%s' is not up or running.\n", name);
		exit (1);
	}
}

/*
 * Local Variables:
 * compile-command: "gcc -Wall -Wstrict-prototypes -o eql_enslave eql_enslave.c"
 * End:
 */

