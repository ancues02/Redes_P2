#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

int main(int argc, char** argv){
    if(argc < 3){
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

    int socket = socket(res->ai_family, res->ai_socktype,0);
    if(socket ==-1){
        std::cerr << "[socket]: creacion socket\n";
        return -1;
    }

    freeaddrinfo(res);

    while(true){
        char buffer[80];

        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct sockadrr cliente;
        __socklen_t longCliente=sizeof(sockaddr);

        int bytes=recvfrom(socket, buffer, 80, 0, &cliente, &longCliente);
        if(bytes == -1){
            std::cerr << "[recvfrom]: lectura mensaje socket\n";
            return -1;
        }

        getnameinfo(&cliente, &longCliente, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICSERV || NI_NUMERICHOST);

    }


    return 0;
}