/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Network.cpp
--
-- PROGRAM: FileTransfer
--
-- FUNCTIONS:
        int sendData(int socket, char * message, int buffsize);
        int connectTCPSocket(int port, char * ip);
        int readSock(int sd, int buffSize, char * buff);
        int setupListen(int port, sockaddr_in * server);
--
-- DATE: 10/03/2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Colin Bose
--
-- PROGRAMMER: Colin Bose
--
-- NOTES: Provides network functions for file transfer - Setting up send/listen sockets and sending data
----------------------------------------------------------------------------------------------------------------------*/
#include "network.h"
#include <errno.h>
#include <QDebug>
int totalSent = 0;
int totalErrs = 0;
Network::Network()
{
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: sendData
--
-- DATE: 10/03/2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Colin Bose
--
-- PROGRAMMER: Colin Bose
--
-- INTERFACE: int sendData(int socket, char * message, int buffSize
                        int socket - sokcet to send to
                        char * message - buffer to write to the socket
                        int buffSize - size of the buffer
--
-- RETURNS: int
--
-- NOTES: Call to send a buffer of data to the specified socket
----------------------------------------------------------------------------------------------------------------------*/
int sendData(int socket, char * message, int buffSize){
    int err;
    err = write(socket, message, buffSize);
    if(err < 0){
        printf("Error writing::\n");
        printf("socket() failed: %s\n", strerror(errno));
        fflush(stdout);
    }
    return err;
}

int sendDataSSL(int socket, char * message, int buffSize, SSL *ssl){
    int err;
    err = SSL_write(ssl, message, buffSize);
    if(err < 0){
        printf("Error writing::\n");
        printf("socket() failed: %s\n", strerror(errno));
        fflush(stdout);
    }
    return err;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: connectTCPSocket
--
-- DATE: 10/03/2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Colin Bose
--
-- PROGRAMMER: Colin Bose
--
-- INTERFACE: int connectTCPSocket(int port, char * ip)
--
-- RETURNS: -1 for failure, or a socket descriptor on success
--
-- NOTES: Call to attempt to connect to a specified ip/port. Returns socket on success
----------------------------------------------------------------------------------------------------------------------*/
int connectTCPSocket(int port, const char * ip){
    int sock;
    struct hostent  * host;
    struct sockaddr_in server;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Cannot create socket");
        exit(1);
    }
    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if ((host = gethostbyname(ip)) == NULL){
        fprintf(stderr, "Unknown server address\n");
        exit(1);
    }
    bcopy(host->h_addr, (char *)&server.sin_addr, host->h_length);
    if (connect (sock, (struct sockaddr *)&server, sizeof(server)) == -1){
        fprintf(stderr, "Can't connect to server\n");
        printf("socket() failed: %s\n", strerror(errno));

        return -1;
    }

    return sock;

}

int connectTCPSocketSSL(int port, const char * ip, SSL *ssl, BIO *sbio){
    int sock;
    struct hostent  * host;
    struct sockaddr_in server;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Cannot create socket");
        exit(1);
    }
    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if ((host = gethostbyname(ip)) == NULL){
        fprintf(stderr, "Unknown server address\n");
        exit(1);
    }
    bcopy(host->h_addr, (char *)&server.sin_addr, host->h_length);
    if (connect (sock, (struct sockaddr *)&server, sizeof(server)) == -1){
        fprintf(stderr, "Can't connect to server\n");
        printf("socket() failed: %s\n", strerror(errno));

        return -1;
    }

    sbio = BIO_new_socket (sock, BIO_NOCLOSE);
    SSL_set_bio (ssl, sbio, sbio);
    if (SSL_connect (ssl) < 0) {
        berr_exit ("SSL Connect Error!");
    }

    return sock;

}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: readSock
--
-- DATE: 10/03/2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Colin Bose
--
-- PROGRAMMER: Colin Bose
--
-- INTERFACE: int readSock(int sd, int buffSize, char * buff)
                    int sd - socket to read
                    int buffSize - size of buffer
                    char * buff - buffer to write to
--
-- RETURNS: returns 0 on client disconnect
--
-- NOTES: Call to read a specified amount of data from socket and stores it in buffer
----------------------------------------------------------------------------------------------------------------------*/
int readSock(int sd, int buffSize, char * buff){
    int n;
    memset(buff, 0, TRANSFERSIZE);
    //("Reading SOCKET: %d \n", sd);
    //fflush(stdout);
    int bytesLeft = buffSize;
    while((n = recv(sd, buff, bytesLeft, 0)) < buffSize){
        if(n == -1){
            if(errno == EAGAIN)
                continue;
            fprintf(stderr, "first failed: %s %d TOTAL ERRS: %d\n", strerror(errno),errno, totalErrs++);
            fflush(stdout);
            return 0;
        }
        if(n == 0){
            return 0;


        }
    buff += n;
    bytesLeft -= n;
    if(bytesLeft ==0)
        return 1;

  }

  return 1;
}

int readSockSSL(int sd, int buffSize, char * buff, SSL *ssl){
    int n;
    memset(buff, 0, TRANSFERSIZE);
    //("Reading SOCKET: %d \n", sd);
    //fflush(stdout);
    int bytesLeft = buffSize;
    while((n = SSL_read(ssl, buff, bytesLeft)) < buffSize){
        if(n == -1){
            if(errno == EAGAIN)
                continue;
            fprintf(stderr, "first failed: %s %d TOTAL ERRS: %d\n", strerror(errno),errno, totalErrs++);
            fflush(stdout);
            return 0;
        }
        if(n == 0){
            return 0;


        }
    buff += n;
    bytesLeft -= n;
    if(bytesLeft ==0)
        return 1;

  }

  return 1;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: setupListen
--
-- DATE: 10/03/2016
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Colin Bose
--
-- PROGRAMMER: Colin Bose
--
-- INTERFACE: setupListen(int port, sockaddr_in * server)
                            int port - port to bind to
                            sockaddr_in * server - sockaddr structure to populate
--
-- RETURNS: returns bound socket or -1 on fail
--
-- NOTES: Call to bind a tcp socket
----------------------------------------------------------------------------------------------------------------------*/

int setupListen(int port, sockaddr_in * server){

    int sock;



    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("Can't create a socket");
        return -1;
    }
    int z;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &z, sizeof(int)) < 0)
        qDebug() << "setsockopt(SO_REUSEADDR) failed";
    // Bind an address to the socket
    bzero((char *)server, sizeof(struct sockaddr_in));
    server->sin_family = AF_INET;
    server->sin_port = htons(port);
    server->sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any client

    if (bind(sock, (struct sockaddr *)server, sizeof(*server)) == -1)
    {
        printf("socket() failed: %s\n", strerror(errno));

        perror("Can't bind name to socket");
        exit(1);
    }
    return sock;
}
int setForwardUDP(int port, const char fIp[], sockaddr_in * client){
    int sd;
    struct	hostent	*hp;
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        {
            perror ("Can't create a socket\n");
            exit(1);
        }

        // Store server's information
         memset((char *) client, 0, sizeof(*client));

        //bzero((char *)&client, sizeof(*client));
        client->sin_family = AF_INET;
        client->sin_port = htons(port);

        if ((hp = gethostbyname(fIp)) == NULL)
        {
            fprintf(stderr,"Can't get server's IP address\n");
            exit(1);
        }
        bcopy(hp->h_addr, (char *)&client->sin_addr, hp->h_length);
        return sd;
}

int  setUdp(int port, const char * servIp, sockaddr_in * serveraddr){
    int sd;
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
       perror ("Can't create a socket\n");
       exit(1);
    }
    memset((char *) serveraddr, 0, sizeof(struct sockaddr_in));
    serveraddr->sin_family = AF_INET;
    serveraddr->sin_port = htons(port);
    serveraddr->sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sd, (struct sockaddr*)serveraddr, sizeof(*serveraddr))==-1){
        printf("error binding");
        fflush(stdout);
        exit(1);
    }

    return sd;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: readSock
-- DATE: 29/10/2016
-- REVISIONS: (Date and Description)
-- DESIGNER: Colin Bose
-- PROGRAMMER: Colin Bose
-- INTERFACE: readSock()
-- RETURNS: int - data read or error no
--
-- NOTES: Call to read a udp socket and store data in given buffer
----------------------------------------------------------------------------------------------------------------------*/

int readSock(int sd, int buffSize, char * buff, sockaddr_in * serveraddr){
    socklen_t slen = sizeof(*serveraddr);
    int err;
    if (err = (recvfrom(sd, buff, buffSize, 0, (struct sockaddr*)serveraddr, &slen))< 0){
        printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));

        return 0;
    }
       return 1;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: sendDataTo
-- DATE: 29/10/2016
-- REVISIONS: (Date and Description)
-- DESIGNER: Colin Bose
-- PROGRAMMER: Colin Bose
-- INTERFACE:
-- RETURNS: int - 0 = error
--
-- NOTES: call to send UDP socket
----------------------------------------------------------------------------------------------------------------------*/

int sendDataTo(int sd, int buffSize, char * buff, sockaddr_in * serveraddr){
    int err = 1;
    if(err = (sendto(sd, buff, buffSize, 0, (struct sockaddr*)serveraddr, sizeof(*serveraddr))) < 0){
        printf("Oh dear, something went wrong with write()! %s\n", strerror(errno));

        return 0;
    }



     return err;
}
void zero(char * buffer, int len){
    memset(buffer, '\0', len);
}

BIO *bio_err=0;
static char *pass;

void generate_eph_rsa_key (SSL_CTX *ctx)
{
    RSA *rsa;
    rsa=RSA_generate_key(512,RSA_F4,NULL,NULL);
    if (!SSL_CTX_set_tmp_rsa(ctx,rsa))
        berr_exit("Couldn't set RSA key");

    RSA_free(rsa);
}

/* A simple error and exit routine*/
int err_exit(char *string)
{
    fprintf(stderr,"%s\n",string);
    exit(0);
}

/* Print SSL errors and exit*/
int berr_exit(char *string)
{
    BIO_printf(bio_err,"%s\n",string);
    ERR_print_errors(bio_err);
    exit(0);
}

/*The password code is not thread safe*/
static int password_cb (char *buf, int num, int rwflag, void *userdata)
{
    if(num<strlen(pass)+1)
      return(0);

    strcpy(buf,pass);
    return(strlen(pass));
}

static void sigpipe_handle (int x)
{

}

SSL_CTX *initialize_ctx (char *keyfile, char *password)
{
    SSL_CTX *ctx;

    if(!bio_err){
      /* Global system initialization*/
      SSL_library_init();
      SSL_load_error_strings();

      /* An error write context */
      bio_err=BIO_new_fp(stderr,BIO_NOCLOSE);
    }

    /* Set up a SIGPIPE handler */
    signal(SIGPIPE,sigpipe_handle);

    /* Create our context*/
    ctx=SSL_CTX_new( SSLv23_method() );

    /* Load our keys and certificates*/
    if(!(SSL_CTX_use_certificate_file(ctx,keyfile,SSL_FILETYPE_PEM)))
      berr_exit("Couldn't read certificate file");

    pass=password;
    SSL_CTX_set_default_passwd_cb(ctx,password_cb);
    if(!(SSL_CTX_use_PrivateKey_file(ctx,keyfile,SSL_FILETYPE_PEM)))
      berr_exit("Couldn't read key file");

    /* Load the CAs we trust*/
    if(!(SSL_CTX_load_verify_locations(ctx,CA_LIST,0)))
      berr_exit("Couldn't read CA list");
    SSL_CTX_set_verify_depth(ctx,1);

    /* Load randomness */
    if(!(RAND_load_file(RANDOM,1024*1024)))
      berr_exit("Couldn't load randomness");

    return ctx;
}

void destroy_ctx(SSL_CTX *ctx)
{
    SSL_CTX_free(ctx);
}
