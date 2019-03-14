

//we initialize the clients to be listening on localhost: port numbers are essentially arbitrarily chosen
#include <zconf.h>
#include "TCP_client.hpp"


TCP_client::TCP_client(int port_number, int next_port, std::string identifier) {
    id = identifier;
    initialize_sockets(port_number, next_port);
    //this needs to change when neighbor is shut down -> token of type (TOK_SHUTDOWN)
    my_listening_port = port_number;
    hello_protocol(port_number == next_port);
    std::cout << "Client has initialized succesfully" << std::endl;
    TCP_client::run();
}

void TCP_client::run() {
    //accept connection and receive tokens in a loop

    SOCKET predecessor_socket = accept_connection();

    std::cout << "[DEBUG]: Connection accepted" << std::endl;
    //Run endless loop
    do {
        //TODO -> instead of sleep, try blocking recv
        int received_bytes = recv( predecessor_socket, (char *) token_buffer, sizeof(token), 0);
        if (received_bytes < 0) {
            //connection lost. need to re-enter the connection//try to reconnect?
            std::cout << "Error receiving the data. Reason: " << WSAGetLastError() << std::endl;
            sleep(1);

        } else {
            //Here begins Processing of the received token
            std::cout << "I have received the token: " << received_bytes << std::endl;
            if (token_buffer->type == TOK_INIT)
                process_new_client();
            else if (token_buffer->type == TOK_MSG && token_buffer->get_dest() == id)
                process_my_msg();
            else if (token_buffer->type == TOK_MSG && token_buffer->get_dest() != id)
                process_not_my_msg();
            else {
                std::cout << id << "Has received unrecognised token type from " << token_buffer->get_source()
                          << std::endl;
            }
        }

    } while (true);
}

void TCP_client::initialize_sockets(int port_number, int neighbor_port) {

    if ((socket_in = init_tcp_socket()) == INVALID_SOCKET) {
        exit(1);
    }
    if ((socket_out = init_tcp_socket()) == INVALID_SOCKET) {
        exit(1);
    }

    //set neighbor's address
    if (port_number != neighbor_port)
        set_neighbor_port(neighbor_port);

    bind_socket(socket_in, port_number);
    if (socket_in == INVALID_SOCKET) {
        exit(1);
    }

}

//Procedure which should be invoked when client is launched -> client introduces himself to others  (init token)
//The client when initialized becomes last, and it's his responsibility to connect to the new client from "the back"
void TCP_client::hello_protocol(bool first_client) {

    if (first_client) {
        // we don't need now the initialization procedure, because we are the first client.
        last_client = true;
        return;
    } else {
        //TODO -> Connect from the other side to my predecessor to receive messages

        //Destination is same as source - because the token must return to me.
        // We send our port number as message content's so we become neighbor
        token_buffer = new token(TOK_INIT, id, id, std::to_string(my_listening_port));
        // we have to send the token to each client (round the circle), to let them know that we are in
        send_token();
        std::cout << "Hello sent. Waiting for response from peers." << std::endl;
        //after sending token, we try to run.
        run();


    }

}

void TCP_client::send_token() {
    // connect and send
    while (connect(socket_out, (sockaddr *) &neighbor_address, sizeof(sockaddr)) == SOCKET_ERROR) {
        std::cout << "Failed to connect " << std::endl;
    }
    std::cout << "[DEBUG]: Connected to " << neighbor_address.sin_port << std::endl;

    int bytes_sent = send(socket_out, (char *) token_buffer, sizeof(token), 0);

    std::cout << bytes_sent << " bytes sent to: " << neighbor_address.sin_port << std::endl;

}

SOCKET TCP_client::accept_connection() {
    //Listen on our listening port for approaching clients
    if (listen(socket_in, 1) == SOCKET_ERROR) {
        std::cout << "Error listening on a socket" << std::endl;
        exit(1);
    }
    //accept connections incoming on our listening port.
    SOCKET client_socket;
    do {
        std::cout << "[DEBUG]: Waiting for connection on port: " << my_listening_port << std::endl;
        client_socket = accept(socket_in, nullptr, nullptr);
    } while (client_socket == SOCKET_ERROR);
    return client_socket;
}

void TCP_client::set_neighbor_port(int neighbor_port) {

    memset(&neighbor_address, 0, sizeof(neighbor_address));
    neighbor_address.sin_family = AF_INET;
    neighbor_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    neighbor_address.sin_port = htons((u_short) neighbor_port);

}

//TODO -> implement here adding new peers in tokenring
void TCP_client::process_new_client() {
    if (last_client) {
        //explanation: Payload in init token is listening port of new client.
        //Because we are the last client connected,
        //we have to connect to new client and confirm him by sending token_init back to him
        set_neighbor_port(std::stoi(token_buffer->get_payload()));
        last_client = false;
        send_token();
    } else if (token_buffer->get_source() == id) {
        //it is our token returning to us. we do not send it back.
        last_client = true;

    } else {
        // here we are not the last client, so we just pass the token
        send_token();
    }
}

//TODO implement both
void TCP_client::process_my_msg() {
    std::cout << "got my msg" << std::endl;
}

void TCP_client::process_not_my_msg() {
    std::cout << "got not my msg" << std::endl;
}