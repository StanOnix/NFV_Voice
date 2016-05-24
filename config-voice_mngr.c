#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stddef.h>

#define BUFSIZE 2048
#define SERVICE_PORT	21234

int main(int argc, char **argv)
{
	struct sockaddr_in myaddr;	/* our address */
	struct sockaddr_in remaddr;	/* remote address */
	socklen_t addrlen = sizeof(remaddr);		/* length of addresses */
	int recvlen = 0, recvlen1 = 0, recvlen2 = 0;			/* # bytes received */
	int fd, id;				/* our socket */
	CONFIGVOICE_Mdl_s *CfgMldCtx;
	CONFIGVOICE_Callbacks_s	*CallbacksValue;
	CfgMldCtx = malloc( sizeof(CONFIGVOICE_Mdl_s) );
	CallbacksValue = malloc( sizeof(CONFIGVOICE_Callbacks_s) );
	CONFIGVOICE_Status retconfig;
	CONFIGVOICE_Status retconfigCapa;
    CONFIGVOICE_capabilities_s configVoiceCapa;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket\n");
		return 0;
	}

	/* bind the socket to any valid IP address and a specific port */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(SERVICE_PORT);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}

	/* now loop, receiving data and printing what we received */
	for (;;) {
		
		printf("waiting on port %d\n", SERVICE_PORT); /** To delete after check **/
		recvlen = recvfrom(fd, &id, sizeof(int), 0, (struct sockaddr *)&remaddr, &addrlen);
		if (id == 1)
		{
			recvlen = recvfrom(fd, CfgMldCtx, sizeof(CONFIGVOICE_Mdl_s), 0, (struct sockaddr *)&remaddr, &addrlen);
		}
		if (id == 2)
		{
			recvlen1 = recvfrom(fd, CallbacksValue, sizeof(CONFIGVOICE_Callbacks_s), 0, (struct sockaddr *)&remaddr, &addrlen);
		}		
		retconfig = CONFIGVOICE_Init(CfgMldCtx, CallbacksValue);
		recvlen2 = recvfrom(fd, &id, sizeof(int), 0, (struct sockaddr *)&remaddr, &addrlen);
		if (id == 3)
		{
			if (sendto(fd, &retconfig, sizeof(CONFIGVOICE_Status), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
			{
				perror("sendto");
			}	
		}
		if (id == 4)
		{
			recvlen1 = recvfrom(fd, &configVoiceCapa, sizeof(CONFIGVOICE_capabilities_s), 0, (struct sockaddr *)&remaddr, &addrlen);
			retconfigCapa = CONFIGVOICE_SetCapabilities(&configVoiceCapa);
			if (sendto(fd, &retconfigCapa, sizeof(CONFIGVOICE_capabilities_s), 0, (struct sockaddr *)&remaddr, addrlen) < 0)
			{
				perror("sendto");
			}
		}
		
		

	}
	
	/* never exits */
}

