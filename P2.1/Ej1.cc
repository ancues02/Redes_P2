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

    memset((void*) &hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], nullptr, &hints, &res);

    if(rc != 0){
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << "\n";
        return -1;
    }

    for(auto i = res; i != nullptr; i = i->ai_next){
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];
        getnameinfo(i->ai_addr, i->ai_addrlen,host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
        //std::cout <<"Host: " << host << " Port: "<< serv << "\n";        
        std::cout << host << " "<< serv << "\n";        
    }

    return 0;
}