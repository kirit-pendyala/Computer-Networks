/* 
 * udpserver.c - A simple UDP echo server 
 * usage: udpserver <port>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include<time.h>
#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char **argv) {
  int sockfd; /* socket */
  int portno; /* port to listen on */
  int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  struct hostent *hostp; /* client host info */
  char buf[BUFSIZE]; /* message buf */
  char *hostaddrp; /* dotted decimal host addr string */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */
  
  //new define
  struct hostent *myent;   
  char buf3[BUFSIZE];//To copy the message from client
  char buf5[BUFSIZE]; // TO store the result from gethostbyaddres function
  int len = 0;
  char *buf2;
  struct in_addr  myen;
  long int *add;
  char h[5][30],ip[5][30];//to store IP and host name values(Acts like a Cache)
  int count = 0;// To maintain as circular list
  time_t rawtime;
//  struct tm *timeinfo;
  int cmp = 0;// To check the hostname is present in Cache or not
  long int ttt[5];
  time_t seconds;// To check the time i.e seconds
   /* check command line arguments 
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);

  /* 
   * socket: create the parent socket 
   */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
	     (const void *)&optval , sizeof(int));

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)portno);
  printf("Server port number: %hu (%d)\n",(unsigned short)serveraddr.sin_port,portno);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(sockfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    error("ERROR on binding");

  /* 
   * main loop: wait for a datagram, then echo it
   */
  clientlen = sizeof(clientaddr);
  while (1)
 {

    /*
     * recvfrom: receive a UDP datagram from a client
     */
    bzero(buf, BUFSIZE);
    n = recvfrom(sockfd, buf, BUFSIZE, 0,
		 (struct sockaddr *) &clientaddr, &clientlen);
    if (n < 0)
      error("ERROR in recvfrom");
	
	seconds = time(NULL);
    /* 
     * gethostbyaddr: determine who sent the datagram
     */

    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 
			  sizeof(clientaddr.sin_addr.s_addr), AF_INET);
    

    if (hostp == NULL)
      error("ERROR on gethostbyaddr");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
      error("ERROR on inet_ntoa\n");
    printf("server received datagram from %s (%s)\n", 
	   hostp->h_name, hostaddrp);
    printf("client port number: %d\n",clientaddr.sin_port);
    printf("server received %d/%d bytes: %s\n", (int)strlen(buf), n, buf);

    int strl = strlen(buf);
    bzero(buf3,BUFSIZE);
    bzero(buf5,BUFSIZE);
    strncpy(buf3,buf,strl-1);   // copy the result from buffer which is sent from client
  //This for loop is used to check whether hostname is present in cache or not
    for(int i=0;i<5;i++)
    {
	char temp[30];
	strcpy(temp,h[i]);
	if(!strcmp(temp,buf3))
	{
		cmp = i+1;// to idetify that match is found and store the index value
		break;
        }
    }
    // if cmp > 0 then it is found in cache
    if(cmp > 0)
    {
	char ltime[30];
	time(&rawtime);
	seconds = time(NULL);
	long int lde = seconds % 60;// gets the current seconds 
	long int x;
	x = ttt[cmp-1];
	//if the time is less than 10 seconds
	if(x < 10)
	{
//		printf("difff: %ld\n",(lde - x));
//		printf("current time : %ld\n",lde);
		printf("Server has %s in cache\n",buf3);
		printf("Server sends %s to client\n",ip[cmp-1]);
		strcpy(buf,"IP: ");
		printf("ip: %s\n",ip[cmp-1]);
		strcat(buf,ip[cmp-1]);// fetching the data from cache
		cmp = 0;// makesure to assign 0 ..
	}
	//if the time is more than 10 seconds
	else
	{
		printf("Server has %s in cache,but invalid\n",buf3);
		printf("Server requests to DNS server\n");		
		myent =  gethostbyname(buf3);
		//getting the ip address by using gethostbyname
	        for(int i=0;myent->h_addr_list[i];i++)
  	        {
	       		strcpy(buf5,inet_ntoa(*(struct in_addr*)myent->h_addr_list[i]));
        	}
		printf("Server gets IP address (%s)\n",buf5);
		printf("Server saves %s %s in cache\n",buf3,buf5);
		printf("Server sends %s to client\n",buf5);
		strcpy(buf,"IP: ");
		strcat(buf,buf5);
		ttt[count] = seconds % 60;
		strcpy(ip[count],buf5);
		strcpy(h[count],buf3);
		count = (count + 1) % 5;// to make it circular list
		cmp = 0;//makesure to assigh for 0 ..
	}
	
     }
	// if the hostname is not there in cache
     else
     {
	   	myent =  gethostbyname(buf3);
		// getting the hostname by using ip address
                for(int i=0;myent->h_addr_list[i];i++)
	        {
	           	strcpy(buf5,inet_ntoa(*(struct in_addr*)myent->h_addr_list[i]));
        	}
		printf("Server has no %s in a cache\n",buf5);
		printf("Server requests to the DNS Server\n");
		printf("Server gets host name address(%s)\n",buf3);
		printf("Server saves in %s %s in cache\n",buf3,buf5);
		printf("Server sends %s sends to client\n",buf5);
			
		ttt[count] = seconds % 60;
		strcpy(ip[count],buf5);
		strcpy(h[count],buf3);
		strcpy(buf,"IP: ");
		strcat(buf,buf5);
		count = (count + 1) % 5;//to make it circular list
		cmp=0;
     }

    /* 
     * sendto: echo the input back to the client 
     */
    n = sendto(sockfd, buf, strlen(buf), 0, 
	       (struct sockaddr *) &clientaddr, clientlen);
    if (n < 0) 
      error("ERROR in sendto");
  }
}

