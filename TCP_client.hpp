//
// Created by Timelock on 11.03.2019.
//

#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP

#include "TokenRingUtils.hpp"


class TCP_client {
private:
    SOCKET socket_out;
    std::string id;
    token token_buffer;
    void initialize_sockets(int port_number, protocol_type protocol, bool first_client);
public:
    TCP_client(int port_number, std::string identifier, protocol_type protocol, bool first_client);
    void run();
};


#endif //TCP_CLIENT_HPP
