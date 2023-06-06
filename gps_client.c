/* gps_client.c
 * 
 * This code originates from an example
 * on http://www.expertsforge.com/Programming/Tutorial-87.asp
 *
 */

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close() */
#include <string.h> /* memset() */

#include "gps_client.h"

#define LOCAL_SERVER_PORT 4950
int getGPS( gps_position_t * pos_data_ptr, int id) {
   
	int sd, rc, n, cliLen;
	int match;
	struct sockaddr_in cliAddr, servAddr;
	
		
	/* socket creation */
	sd=socket(AF_INET, SOCK_DGRAM, 0);
	if(sd<0) {
		printf(" cannot open socket \n");
		return(1);
	}

	/* bind local server port */
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(LOCAL_SERVER_PORT);
	rc = bind (sd, (struct sockaddr *) &servAddr,sizeof(servAddr));
	
	if(rc<0) {
		printf(" cannot bind port number %d \n", LOCAL_SERVER_PORT);
		return(1);
	}

	printf("  waiting for data on port UDP %u\n",LOCAL_SERVER_PORT);

	pos_data_ptr->px = -1.0;
	pos_data_ptr->py = -1.0;
	pos_data_ptr->pa = -1.0;
	pos_data_ptr->id = -1.0;
	
	match = 0;
	while(match != 1) {

		/* receive message */
		cliLen = sizeof(cliAddr);
		n = recvfrom(sd, pos_data_ptr, sizeof(*pos_data_ptr), 0,(struct sockaddr *) &cliAddr, (unsigned *)&cliLen);

		if(n<0) {
			printf("  cannot receive data \n");
			continue;
		}

		/* print received message */
		if( pos_data_ptr->id == id ) {
			//printf("From %s:UDP%u : \n",inet_ntoa(cliAddr.sin_addr),ntohs(cliAddr.sin_port));
			//printf("ID: %d PX: %d  PY: %d  PA: %d\n", pos_data_ptr->id, pos_data_ptr->px, pos_data_ptr->py, pos_data_ptr->pa);
		match = 1;
		}
	}
	close(sd);

	return 0;
}

