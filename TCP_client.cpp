

//we initialize the clients to be listening on localhost: port numbers are essentially arbitrarily chosen
#include <zconf.h>
#include "TCP_client.hpp"

class TCP_client {
private:
    //Private fields
    SOCKET socket_out;
    std::string id;
    token *token_buffer;

    //Private methods
    void initialize_sockets(int port_number, protocol_type protocol, bool first_client) {

        if (protocol == UDP) {
            if ((socket_out = init_udp_socket()) == INVALID_SOCKET) {
                exit(1);
            }
        } else if (protocol == TCP) {
            if ((socket_out = init_tcp_socket()) == INVALID_SOCKET) {
                exit(1);
            }
        }

        bind_socket(socket_out, port_number);
        if (socket_out == INVALID_SOCKET) {
            exit(1);
        }
    }

    void initialize_token(bool first_client){

        if (first_client)
            token_buffer = new token(TOK_MSG);
        else
            //todo - resolve connecting new client with init token
            token_buffer = new token(TOK_INIT);
    }

public:
    //port_number -> describes which port the client is listening on
    TCP_client(int port_number, std::string identifier, protocol_type protocol, bool first_client) {
        id = identifier;
        initialize_sockets(port_number, protocol, first_client);
        initialize_token(first_client);
        std::cout << "Client has initialized succesfully" << std::endl;

    }
    void run(){
        std::cout<<"Client " << id << " has started up"<<std::endl;
        sleep(10);
    }


};