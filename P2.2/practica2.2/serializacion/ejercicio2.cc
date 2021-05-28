#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


//el comando od representa en octal por defecto el archivo
//
class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t data_size=MAX_NAME * sizeof(char) + 2 * sizeof(int16_t);
        alloc_data(data_size);

        char* tmp = _data;

        memcpy(tmp, name, MAX_NAME*sizeof(char));
        tmp+=MAX_NAME * sizeof(char);

        memcpy(tmp, &x, sizeof(int16_t));
        tmp+=sizeof(int16_t);

        memcpy(tmp, &y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        char* tmp = data;

        memcpy(name, tmp, MAX_NAME*sizeof(char));
        tmp+=MAX_NAME * sizeof(char);

        memcpy(&x, tmp, sizeof(int16_t));
        tmp+=sizeof(int16_t);

        memcpy(&y, tmp, sizeof(int16_t));

        return 0;
    }


public:
static const size_t MAX_NAME = 20;
    char name[MAX_NAME];

    int16_t x;
    int16_t y;

};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    int fd = open("./data_jugador", O_CREAT | O_TRUNC | O_RDWR, 0666);//se consigue el fichero
    if(fd == -1 ){
        std::cerr << "[open]: error en open del archivo ./data_jugador\n";
        return -1;
    }
    one_w.to_bin();//se serializa

    //write(fd, one_w.data(),one_w.size());
    if( write(fd, one_w.data(),one_w.size()) == -1){//guardar en el archivo
        std::cerr << "[write]: error en write\n";
        return -1;
    }
    if(close(fd) == -1){
        std::cerr << "[close]: error al cerrar el archivo \n";
        return -1;
    }


    fd = open("./data_jugador", O_RDONLY, 0444);//se consigue el fichero
    if(fd == -1 ){
        std::cerr << "[open]: error en open del archivo ./data_jugador\n";
        return -1;
    }
    char* data ;

    if(read(fd,data,one_w.size()) == -1){
        std::cerr << "[read]: error en read\n";
        return -1;
    }

    one_r.from_bin(data);

    std::cout << "Jugador des-serializado:  name = " << one_r.name << 
    " pos x: "<< one_r.x << " pos y: " << one_r.y <<"\n";

    if(close(fd) == -1){
        std::cerr << "[close]: error al cerrar el archivo \n";
        return -1;
    }

    return 0;
}

