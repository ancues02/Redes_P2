#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <time.h>

int main(int argc, char** argv){
            
  
    if(argc < 3){
        std::cerr << "Se necesitan mas argumentos\n";
        return -1;
    }
    struct addrinfo hints;
    struct addrinfo *res;
    //reservo memoria para un addrinfo 
    memset((void*) &hints, 0, sizeof(addrinfo));

    //IPV4 
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;//TCP

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
    if(rc != 0){
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << "\n";
        return -1;
    }

    int _socket = socket(res->ai_family, res->ai_socktype,0);//creo mi socket
    if(_socket ==-1){
        std::cerr << "[socket]: creacion socket\n";
        return -1;
    }
    //no hace falta hacer bind porque ya lo hace el servidor
    

    
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr cliente;
    socklen_t longCliente=sizeof(sockaddr);

    if(connect(_socket, res->ai_addr, res->ai_addrlen) == -1){//realizo la conexion al servidor
        std::cerr << "[connect] Error en connect\n";
        return -1;
    }

    char buffer[80];
    int tam=0;

    while(true){
        std::cin >> buffer;
        tam = send(_socket,buffer,strlen(buffer),0);
        if(tam == -1){
            std::cerr << "[send] Error al mandar el mensaje\n";
            return -1;
        }
        int bytes = recv(_socket, buffer, 80, 0);
        if(bytes==-1){
            std::cerr << "[recv] Error al recibir el mensaje\n";
            return -1;
        }
        if( buffer[0] == 'Q' && buffer[1]=='\0')//cierre del while si recibo una Q
            break;
        else
            std::cout << buffer<<"\n";
        
    }
    freeaddrinfo(res);
    if(close(_socket)==-1){
        std::cerr << "[close]: cierre socket\n";
        return -1;
    }  
     
    return 0;
}