#include<stdio.h>
#include<string.h>    
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    

#include <fcntl.h>  /* O_RDWR */
#include <stdlib.h> 
#include <sys/ioctl.h> /* ioctl() */

/* includes for struct ifreq, etc */
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>

// function for configuring tunnel interface.
int tun_open(char *devname)
{
    struct ifreq ifr;
    int fd, err;

    if ( (fd = open("/dev/net/tun", O_RDWR)) == -1 ) {
        perror("open /dev/net/tun");
        exit(1);
    }

    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = (IFF_TUN | IFF_NO_PI);
    strncpy(ifr.ifr_name, devname, IFNAMSIZ);

    if ( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) == -1 ) {
        perror("ioctl TUNSETIFF");close(fd);exit(1);
    }
    // After the ioctl call the fd is "connected" to tun device specified by devname.

    return fd;
}

 
// driver function
int main(int argc , char *argv[])
{

    // Prepare the connection to the 'asa0' TUN interface first.
    int fd, nbytes;
    char buf[1600];

    // opening first TUN interface
    fd = tun_open("asa0") ;
    printf("Device asa0 opened\n");

    // preparing to accept TCP connection from the tun-router-client
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];
     
    //Creating socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    
    // error in creating in socketing
    if (socket_desc == -1)
    {
        printf("Socket Can't be created");
    }
    puts("Socket created");
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
    
    
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("Couldn't bind");
        return 1;
    }
    puts("bind done");
     
    listen(socket_desc , 3);
     
    //Accept an incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");
     
    //Receive a message from client
    while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 )
    {
        printf("received %d bytes\n", read_size);

    	// writing to the TUN file descriptor
    	nbytes = write(fd, client_message, read_size);
    	// nbytes = write(fd, buf, read_size); 
        printf("Wrote %d bytes to TUN file descriptor\n", nbytes);

	    nbytes = 0;
    	memset(client_message, 0, sizeof(client_message));

    	// read from the TUN file descriptor
	    nbytes = read(fd, buf, sizeof(buf)); 
        printf("Read %d bytes from asa0\n", nbytes);

    	// writing data back to the client socket
    	write(client_sock , buf, nbytes);

        memset(client_message, 0, sizeof(client_message));
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
     
    return 0;
}
