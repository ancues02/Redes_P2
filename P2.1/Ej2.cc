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
    bind(_socket,res->ai_addr,res->ai_addrlen);

    freeaddrinfo(res);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr cliente;
    socklen_t longCliente=sizeof(sockaddr);

    char buffer;

    int bytes=recvfrom(_socket, buffer, 80, 0, &cliente, &longCliente);
    if(bytes == -1){
        std::cerr << "[recvfrom]: lectura mensaje socket\n";
        return -1;
    }
    getnameinfo(&cliente, longCliente, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICSERV || NI_NUMERICHOST);
    //si es q se acaba la conexion
    while(buffer != 'q'){


        if(buffer=='d'){
            /*int send = sendto(_socket, date(), bytes, 0, &cliente, longCliente);
            if(send == -1){
                std::cerr << "[sendto]: error al mandar el mensaje\n";
                return -1;
            }*/
        }
        else if(buffer=='t'){
            time_t _time;
            struct tm* info;
            time(&_time);
            info= localtime(&_time);
            char* hora;
            int horas= strftime(hora,255, "%X %P" , info);
            int send = sendto(_socket, horas, bytes, 0, &cliente, longCliente);
            if(send == -1){
                std::cerr << "[sendto]: error al mandar el mensaje\n";
                return -1;
            }
        }
        else{
            std::cout << "Comando no soportado" << buffer <<"\n";
            int send = sendto(_socket, "Comando no declarado", bytes, 0, &cliente, longCliente);
            if(send == -1){
                std::cerr << "[sendto]: error al mandar el mensaje\n";
                return -1;
            }
        }


        /*std::cout << "Host: " << host << "Port: "<< serv << "\n";
        std::cout << "Datos: " << buffer << "\n";*/

    }

    close(_socket);

    return 0;
}