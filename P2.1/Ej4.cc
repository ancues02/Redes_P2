#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
//Solo acepta un cliente
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
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

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

    int b=bind(_socket,res->ai_addr,res->ai_addrlen);
    if(b==-1){
        std::cerr << "[bind]: uso de bind\n";
        return -1;
    }
    freeaddrinfo(res);

    if(listen(_socket,1) == -1){
        std::cerr << "[listen]: uso de listen\n";
        return -1;
    }

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr cliente;
    socklen_t longCliente=sizeof(sockaddr);

    int cliente_socket=accept(_socket, &cliente, &longCliente);

    if(cliente_socket==-1){
        std::cerr << "[accept] Error en accept\n";
        return -1;
    }

    getnameinfo(&cliente, longCliente, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICSERV || NI_NUMERICHOST);

    std::cout << "Conexion desde " << host << " " << serv << "\n"; 

    char buffer[80];
    //si es q se acaba la conexion
    while(true){
        int bytes=recv(cliente_socket, buffer, 79, 0);
        if(bytes == 0){
            std::cout << "Fin conexion\n";
            break;
        }
        else if(bytes == -1){
            std::cout << "[recv] Error en recv\n";
            return -1;
        }

        if(send(cliente_socket,buffer, bytes,0) < 0){
            std::cout <<"[sendto]: Error al mandar el mensaje";
            return -1;
        }
       
    }
    if(close(_socket)==-1){
        std::cerr << "[close]: cierre socket\n";
        return -1;
    }  
    if(close(cliente_socket)==-1){
        std::cerr << "[close]: cierre socket cliente\n";
        return -1;
    }    
    return 0;
}