#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

int main(int argc, char** argv){
    if(argc < 4){
        std::cerr << "Se necesitan mas argumentos\n";
        return -1;
    }
    struct addrinfo hints;
    struct addrinfo *res;
    //reservo memoria para un addrinfo con valor 0
    memset((void*) &hints, 0, sizeof(addrinfo));

    //cojo IPV4 
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;//UDP
    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if(rc != 0){
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << "\n";
        return -1;
    }

    int _socket = socket(res->ai_family, res->ai_socktype,0);
    if(_socket ==-1){
        std::cerr << "[socket]: creacion socket\n";
        return -1;
    }

//************************a partir de aqui es lo diferente del ejercicio2***********************//
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    char buffMens[40];
    struct sockaddr servidor;
    socklen_t longServidor=sizeof(sockaddr);

    getnameinfo(&servidor, longServidor, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICSERV || NI_NUMERICHOST);

    int send = sendto(_socket, argv[3], 2, 0, res->ai_addr, res->ai_addrlen);
    if(send == -1){
        std::cerr << "[sendto]: error al mandar el mensaje\n";
        return -1;
    }
    int bytes=recvfrom(_socket, buffMens, 40, 0, &servidor, &longServidor);
    if(bytes == -1){
        std::cerr << "[recvfrom]: lectura mensaje socket\n";
        return -1;
    }
    
    std::cout << buffMens<<"\n";
    //cierre
    freeaddrinfo(res);
    if(close(_socket)==-1){
        std::cerr << "[close]: cierre socket\n";
        return -1;
    }
    return 0;
}