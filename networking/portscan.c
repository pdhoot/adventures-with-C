#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <strings.h>

void fatal(char* message)
{
	perror(message);
	exit(1);
}

int main(int argc , char* argv[])
{
	int i, count=0;
	struct sockaddr_in dest_addr;
	struct hostent* dest;
	socklen_t len = sizeof(struct sockaddr);
	if(argc<2)
	{
		fatal("Usage: ./portscan <destIP>");
	}

	if((dest=gethostbyname(argv[1]))==NULL)
	{
		if((dest_addr.sin_addr.s_addr=inet_addr(argv[1]))==-1)
		{
			fatal("Failed to convert arg1 to ip address");
		}
	}
	else
	{
		bcopy(dest->h_addr , &dest_addr.sin_addr.s_addr, dest->h_length);
	}

	printf("%9s%7s\n", "Port" , "Status");
	for(i=0 ; i<65536 ; i++)
	{
		int sock;
		if((sock=socket(AF_INET , SOCK_STREAM , 0))==-1)
		{
			fatal("Couldn't create the socket");
		}

		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(i);

		if(connect(sock , (struct sockaddr*)&dest_addr , len)==-1)
		{
			// printf("Port %d closed\n" , i);
		}
		else
		{
			printf("%5d/tcp%5s\n" , i , "open");
			count++;
		}

		close(sock);
	}

	if(count==0)
	{	
		printf("The program couldn't find any open TCP ports on the target host.");
	}

}