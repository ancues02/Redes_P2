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
    int b=bind(_socket,res->ai_addr,res->ai_addrlen);
    if(b==-1){
        std::cerr << "[bind]: uso de bind\n";
        return -1;
    }
    freeaddrinfo(res);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr cliente;
    socklen_t longCliente=sizeof(sockaddr);

    char buffer[1];
    char buffMens[80];
   
    time_t _time;
    struct tm *info;
    time(&_time);
    info= localtime(&_time);
    //si es q se acaba la conexion
    while(buffer[0] != 'q'){

        int bytes=recvfrom(_socket, buffer, 10, 0, &cliente, &longCliente);
        if(bytes == -1){
            std::cerr << "[recvfrom]: lectura mensaje socket\n";
            return -1;
        }
        getnameinfo(&cliente, longCliente, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICSERV || NI_NUMERICHOST);

        if(buffer[0] == 't'){
            int diaSize =strftime(buffMens,255, "%X %p" , info);
            int send = sendto(_socket, buffMens, diaSize+1, 0, &cliente, longCliente);
            if(send == -1){
                std::cerr << "[sendto]: error al mandar el mensaje\n";
                return -1;
            }
        }
        else if(buffer[0] == 'd'){
            int horaSize = strftime(buffMens,255, "%Y-%m-%d" , info);
            int send = sendto(_socket, buffMens, horaSize+1, 0, &cliente, longCliente);
            if(send == -1){
                std::cerr << "[sendto]: error al mandar el mensaje\n";
                return -1;
            }
        }
        else if (buffer[0] != 'q'){            
             int send = sendto(_socket, "Comando no declarado", 21, 0, &cliente, longCliente);
            if(send == -1){
                std::cerr << "[sendto]: error al mandar el mensaje\n";
                return -1;
            }
        }
        std::cout << bytes << " bytes de "<< host<<":"<<serv<<"\n"; 
    }
    std::cout << "Saliendo...\n";
    if(close(_socket)==-1){
        std::cerr << "[close]: cierre socket\n";
        return -1;
    }    
    return 0;
}