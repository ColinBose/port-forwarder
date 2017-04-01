#ifndef NETWORK_H
#define NETWORK_H

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
//#define TRANSFERSIZE 1000
#define STARTERLENGTH 200
#define LISTSIZE 10000
#define PORT 7000
#define TRANSFERSIZE 2000

#define CA_LIST     "../test/ca.pem"
#define DHFILE      "../test/dh1024.pem"
#define RANDOM      "../test/random.pem"
#define CLT_KEYFILE "../test/client.pem"
#define SVR_KEYFILE "../test/server.pem"
#define PASSWORD    "password"
static unsigned char s_server_session_id_context = 'A';

class Network
{
public:
    Network();
};
void setUdp(int port, const char * servIp);
int sendData(int socket, char * message, int buffsize);
int sendDataSSL(int socket, char * message, int buffSize, SSL *ssl);
int  setUdp(int port, const char * servIp, sockaddr_in * serveraddr);
int connectTCPSocket(int port, const char *ip);
int readSock(int sd, int buffSize, char * buff);
int readSockSSL(int sd, int buffSize, char * buff, SSL *ssl);
int sendDataTo(int sd, int buffSize, char * buff, sockaddr_in * serveraddr);
int setupListen(int port, sockaddr_in * server);
int readSock(int sd, int buffSize, char * buff, sockaddr_in * serveraddr, int timeout);
void zero(char * buffer, int len);
int setForwardUDP(int port, const char fIp[], sockaddr_in * client);

void generate_eph_rsa_key(SSL_CTX *ctx);
static int password_cb(char *buf,int num,int rwflag,void *userdata);
static void sigpipe_handle(int x);
int berr_exit (char *string);
int err_exit(char *string);

SSL_CTX *initialize_ctx(char *keyfile, char *password);
void destroy_ctx(SSL_CTX *ctx);
#endif // NETWORK_H
