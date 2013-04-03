#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "fft_socket_header.h"
#include "fft_sender.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

	///////////////////////////////////////////////////////////////////////////////////
	int bytesToNextHeader = 5;	// total amount of space (header+data)
	int samplesToNextFFT = 3;	// Num samples to the start of the next FFT
	int ptsPerFFT = 2;			// number of points per FFT 
	int sampFreq = 4;
	
	char fft_data[ptsPerFFT];
	init_fft();
	int header_len = sizeof(struct fft_header);
	
	// Place header in the char array
	char buffer[header_len];
	int i;
	for(char i = 0; i < header_len; i++){
		buffer[i] = hdr[i];
	}
	
	///////////////////////////////////////////////////////////////////////////////////

    // char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    /*
	printf("Please enter the message: ");
	bzero(buffer,256);
    fgets(buffer,255,stdin);
	*/
    ///////////////////////////////////////////////////////////////////////////////////////////
	n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer, header_len);
	
    n = read(sockfd, buffer, 255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
	
	FILE * f =fopen("data.txt","w");
	fwrite(buffer, 1, 255, f);
	fclose(f);
	
	n = write(sockfd, fft_data, strlen(fft_data));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(fft_data, ptsPerFFT);
	///////////////////////////////////////////////////////////////////////////////////////////
    close(sockfd);
    return 0;
}
