#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <time.h>

#define MAX_THREAD 5


class ThreadTCP{
    public:
    ThreadTCP(int socket):_socket(socket){
    }
    //basicamente lo que esta en el while (y sus variables de arriba) del ejercicio 2
    void do_message(){
        
        char buffer[80];
        int tam=0;
        while(true){
            int bytes=recv(_socket, buffer, 79, 0);
            if(bytes == 0){
                std::cout << "Fin conexion\n";
                break;
            }
            else if(bytes == -1){
                std::cout << "[recv] Error en recv\n";
                break;
            }

            if(send(_socket,buffer, bytes,0) < 0){
                std::cout <<"[sendto]: Error al mandar el mensaje";
                break;
            }
            
        }
        if(close(_socket)==-1){
            std::cerr << "[close]: cierre socket cliente\n";
            //return -1;
        }  
    }
    private:
    int _socket;
};



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

    if(listen(_socket,5) == -1){
        std::cerr << "[listen]: uso de listen\n";
        return -1;
    }

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr cliente;
    socklen_t longCliente=sizeof(sockaddr);
    while(true){
        int cliente_socket=accept(_socket, &cliente, &longCliente);

        if(cliente_socket==-1){
            std::cerr << "[accept] Error en accept\n";
            return -1;
        }   
        getnameinfo(&cliente, longCliente, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICSERV || NI_NUMERICHOST);
        std::cout << "Conexion desde " << host << " " << serv << "\n"; 
        
        ThreadTCP* miThread=new ThreadTCP(_socket);

        std::thread([&miThread](){ miThread->do_message(); delete miThread;  }).detach();
    
    }
    if(close(_socket)==-1){
        std::cerr << "[close]: cierre socket\n";
        return -1;
    }  
      
    return 0;
}