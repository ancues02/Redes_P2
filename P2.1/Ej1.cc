#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

int main(int argc, char** argv){
    if(argc < 2){
        std::cerr << "Se necesitan mas argumentos\n";
        return -1;
    }
    struct addrinfo hints;
    struct addrinfo *res;
    //reservo memoria para un addrinfo con valor 0
    memset((void*) &hints, 0, sizeof(addrinfo));

    //cojo tanto IPV4 como IPV6
    hints.ai_family = AF_UNSPEC;

    int rc = getaddrinfo(argv[1], nullptr, &hints, &res);

    if(rc != 0){
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << "\n";
        return -1;
    }

    for(auto i = res; i != nullptr; i = i->ai_next){
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        int nameInfo=getnameinfo(i->ai_addr, i->ai_addrlen,host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        if(nameInfo != 0){
            std::cerr < "Error al usar getnameinfo\n";
        }
        std::cout << "Host: "<< host  <<" Family " << i->ai_family << " SocketType: "<< i->ai_socktype <<"\n";        
    }

    return 0;
}