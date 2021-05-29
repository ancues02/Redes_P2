#include "Chat.h"

//funciona bien salvo que no hace logout
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* tmp=_data;

    memcpy(tmp, &type, sizeof(u_int8_t));
    tmp += sizeof(u_int8_t);

    memcpy(tmp, nick.c_str(), 8 * sizeof(char));
    tmp += 8 * sizeof(char);

    memcpy(tmp, message.c_str(), 80 * sizeof(char));

}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    char* tmp = _data;

    memcpy(&type,tmp, sizeof(u_int8_t));

    tmp+=sizeof(u_int8_t);

    nick = tmp;
    tmp += 8 * sizeof(char);

    message = tmp;

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    std::cout << "nuevo cliente\n";
    
    ChatMessage msg;
    Socket* client;
    while (true)
    {
        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */
       
        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
    

        if(socket.recv(msg, client) != 0){
            std::cerr << "[recv]: error en recv do_message\n";
             continue;
        }
        client->bind();

        switch(msg.type){
            case ChatMessage::LOGIN:
                clients.push_back(std::move(std::unique_ptr<Socket>(client)));                
            break;
            case ChatMessage::LOGOUT:
            {          
               auto it = clients.begin();
               while(it != clients.end() ) {
                   if(*client == *it->get()){
                        clients.erase(it);
                        it->release();
                        break;
                   }
                   ++it;
                }
            }
            break;
            case ChatMessage::MESSAGE:
            {
                for(auto it = clients.begin(); it != clients.end() ; ++it){
                    if(!(*client == *it->get())){
                        socket.send(msg,*it->get());
                    }                    
                }
            }
            break;
        }

    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;
    std::cout<<"login\n";
    if(socket.send(em, socket) != 0){
        std::cerr << "[send]: error en login\n";
        return;
    }
}

void ChatClient::logout()
{
    //manda mensaje de logout al servidor
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    if(socket.send(em, socket) != 0){
        std::cerr << "[send]: error en logout\n";
        return;
    }
}

void ChatClient::input_thread()
{
    while (true)
    {

        // Leer stdin con std::getline
        // Enviar al servidor usando socket
        std::string msg;
        std::getline(std::cin, msg);

        ChatMessage em(nick, msg);
        em.type = ChatMessage::MESSAGE;

        if(socket.send(em, socket) != 0) {
            std::cerr << "[send]: error al mandar el mensaje\n";
            return ;
        }
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        ChatMessage msg;
        if(socket.recv(msg) != 0){
            std::cerr << "[recv]: error al recibir el mensaje\n";
            continue;
        }
        std::cout  << msg.nick << ": " << msg.message <<"\n";
    }
}

