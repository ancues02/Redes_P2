#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <thread>

#define MAX_THREAD 5

//Cada mensaje lo recibe un hilo diferente. Hay un maximo de 5 hilos. 
//El cliente se cierra solo si el cliente se escribe la q.

class ThreadUDP{
    public:
    ThreadUDP(int socket):_socket(socket){

    }
    //basicamente lo que esta en el while (y sus variables de arriba) del ejercicio 2
    void do_message(){
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

        while(true){
            int bytes=recvfrom(_socket, buffer, 10, 0, &cliente, &longCliente);
            sleep(3);
            if(bytes == -1){
                std::cerr << "[recvfrom]: lectura mensaje socket\n";
                continue;//return -1;//no funciona, saltamos la linea
            }
            getnameinfo(&cliente, longCliente, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICSERV || NI_NUMERICHOST);

            if(buffer[0] == 't'){
                int diaSize =strftime(buffMens,255, "%X %p" , info);
                int send = sendto(_socket, buffMens, diaSize+1, 0, &cliente, longCliente);
                if(send == -1){
                    std::cerr << "[sendto]: error al mandar el mensaje\n";
                    continue;//return -1;//no funciona, saltamos la linea
                }
            }
            else if(buffer[0] == 'd'){
                int horaSize = strftime(buffMens,255, "%Y-%m-%d" , info);
                int send = sendto(_socket, buffMens, horaSize+1, 0, &cliente, longCliente);
                if(send == -1){
                    std::cerr << "[sendto]: error al mandar el mensaje\n";
                    continue;//return -1;//no funciona, saltamos la linea
                }
            }
            else if (buffer[0] != 'q'){            
                int send = sendto(_socket, "Comando no declarado", 21, 0, &cliente, longCliente);
                if(send == -1){
                    std::cerr << "[sendto]: error al mandar el mensaje\n";
                    continue;//return -1;//no funciona, saltamos la linea
                }
            }
            std::cout << "Thread " << std::this_thread::get_id() << " : " << bytes << " bytes de "<< host<<":"<<serv<<"\n"; 
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


    for(int i=0; i< MAX_THREAD; ++i){
        ThreadUDP* miThread=new ThreadUDP(_socket);

        std::thread([&miThread](){ miThread->do_message(); delete miThread;  }).detach();

    }
    //char* buff;
    char buff[80];
    //si es q se acaba la conexion
    do{
        std::cin >> buff;
    }while(buff[0]!='q' && buff[1]!='\0');
   
    std::cout << "Saliendo...\n";
    if(close(_socket)==-1){
        std::cerr << "[close]: cierre socket\n";
        return -1;
    }
    return 0;
}