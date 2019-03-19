

//we initialize the clients to be listening on localhost: port numbers are essentially arbitrarily chosen
#include <zconf.h>
#include <cstring>
#include <arpa/inet.h>
#include <ctime>
#include "TCP_client.hpp"

TCP_client::TCP_client(int port_number, int next_port, std::string identifier) {
    id = identifier;
    initialize_sockets(port_number, next_port);
    //this needs to change when neighbor is shut down -> token of type (TOK_SHUTDOWN)
    my_listening_port = port_number;
    neighbor_port = next_port;
    token_buffer = (token *) malloc(sizeof(token));
    want_to_send = false;
    hello_protocol(port_number == next_port);
    std::cout << "Client has initialized succesfully" << std::endl;
    TCP_client::run(port_number == next_port);
}

void TCP_client::run(bool initial_client) {
    //accept connection and receive/send tokens in a loop
    do {
        // we need to wait for situation when we have someone to send to
        // if we are initial client, we must send the first MSG token
        bool has_peers = !all_peers.empty();
        if (initial_client && has_peers) {
            std::cout << "[DEBUG]: Sending initial MSG token" << std::endl;
            initial_client = false;
            send_random_message();
        }

        SOCKET predecessor_socket = accept_connection();
        receive_token(predecessor_socket);
        //Passflag case -> we need to just pass the token
        //Second is a very special case for when token needs to
        if (passflag || (!passflag && !want_to_send && token_buffer->type != TOK_INIT)) {
            passflag = false;
            std::cout << "[DEBUG]: Passing the token to neighbour" << std::endl;
            randomize_need_to_send();
            send_token();
        } else if (want_to_send && token_buffer->is_free && has_peers) {
            send_random_message();
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
    //initialize UDP multicast socket

    initialize_multicast();


}

//Procedure which should be invoked when client is launched -> client introduces himself to others  (init token)
//The client when initialized becomes last, and it's his responsibility to connect to the new client from "the back"
void TCP_client::hello_protocol(bool first_client) {

    if (!first_client) {
        //Destination is same as source - because the token must return to me.
        // We send our port number and our desired neighbor port, so predecessor of our new neighbor can adapt,
        // and change the neighbor to us.
        // So format is line old_listening:new_listening

        token_buffer = new token(TOK_INIT, id, id,
                                 std::to_string(neighbor_port) + ":" + std::to_string(my_listening_port));
        // we have to send the token to each client (round the circle), to let them know that we are in
        send_token();
        std::cout << "Hello sent. Waiting for response from peers." << std::endl;
        //after sending token, we try to run (not as initial client).
        run(false);
    }

}

void TCP_client::send_token() {
    //one second delay, because client must hold the token 1s before sending it.
    if (token_buffer->type == TOK_MSG)
        sleep(1);
    // connect and send
    socket_out = init_tcp_socket();
    //todo -> make this a blocking call somehow?
    while (connect(socket_out, (sockaddr *) &neighbor_address, sizeof(sockaddr)) == SOCKET_ERROR) {
        sleep(1);
        std::cout << "Failed to connect " << std::endl;
        perror("Reason: ");
    }
//    std::cout << "[DEBUG]: Connected to " << neighbor_address.sin_port << std::endl;

    ssize_t bytes_sent = send(socket_out, (char *) token_buffer, sizeof(token), 0);

//    std::cout << "[DEBUG]: Token of type " << (token_buffer->type == TOK_INIT ? "TOK_INIT" : "TOK_MSG") << " sent"
//              << std::endl
    close(socket_out);
}

void TCP_client::receive_token(SOCKET from) {

    ssize_t received_bytes = recv(from, (char *) token_buffer, sizeof(token), 0);
    if (received_bytes < 0) {
        //connection lost
        std::cout << "Error receiving the data.";
        perror("Reason: ");
        std::cout << std::endl;
        //retry after 1s
        sleep(1);

    } else {
        //Here begins Processing of the received token
        std::cout << "[DEBUG]: I have received the token." << std::endl;
        log();
        std::string source = token_buffer->get_source();
        std::string dest = token_buffer->get_dest();
        token_type message_type = token_buffer->type;

        if (source != id && !has_peer(source) && !source.empty())
            add_peer(source);


        if (message_type == TOK_INIT)
            process_new_client();
        else if (message_type == TOK_MSG && dest == id && !dest.empty())
            process_my_msg();
        else if (message_type == TOK_MSG && dest != id && !dest.empty())
            process_not_my_msg();
        else if (dest.empty())
            passflag = false;
        else {
            std::cout << id << "Has received unrecognised token type from " << token_buffer->get_source()
                      << std::endl;
        }
    }
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
//        std::cout << "[DEBUG]: Waiting for connection on port: " << my_listening_port << std::endl;
        client_socket = accept(socket_in, nullptr, nullptr);
    } while (client_socket == SOCKET_ERROR);
    return client_socket;
}

void TCP_client::set_neighbor_port(int neigh_port) {
    std::cout << "[DEBUG]: Changing neighbor port to: " << neigh_port << std::endl;
    memset(&neighbor_address, 0, sizeof(neighbor_address));
    neighbor_address.sin_family = AF_INET;
    neighbor_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    neighbor_address.sin_port = htons((u_short) neigh_port);
    neighbor_port = neigh_port;

}

//Method sets passflag for the TOK_INIT token currently processed
void TCP_client::process_new_client() {
    std::string payload = token_buffer->get_payload();
    int old_port = std::stoi(split(payload, ":")[0]);
    int new_port = std::stoi(split(payload, ":")[1]);
    if (token_buffer->get_source() == id) {
        //it is our token returning to us. we do not send it back. it is end of hello protocol.
        std::cout << "[DEBUG]: " << id << ": My own init messsage returned to me." << std::endl;
        passflag = false;
    } else if (neighbor_port == old_port) {
        //explanation: Payload in init token is old:new ports mapping
        set_neighbor_port(new_port);
        passflag = true;
    } else {
        // here we just pass the token through
        passflag = true;
    }
}

void TCP_client::process_my_msg() {
    std::cout << "[DEBUG]: Client of id " << id << " has received message from " + token_buffer->get_source()
              << std::endl << "Contents: " << token_buffer->get_payload() << std::endl;
    //We have received our message.
    //Pass the token (passflag), and make it free
    token_buffer->is_free = true;
    token_buffer->set_dest("");
    token_buffer->set_source("");
    token_buffer->set_payload("");

    passflag = true;

}

void TCP_client::process_not_my_msg() {
    //If we are the source, make the token free.
    //Set passflag to false -> because we
    if (token_buffer->get_source() == id) {
        token_buffer->is_free = true;
        token_buffer->set_dest("");
        passflag = false;
    } else {
        passflag = true;
    }

}

void TCP_client::add_peer(std::string peer_id) {
    std::cout << "[DEBUG]: Adding peer " << peer_id << " to my possible destinations" << std::endl;
    all_peers.push_back(peer_id);
}


void TCP_client::randomize_need_to_send() {
    //Randomly make it true
    if (rand() % 10 == 0)
        want_to_send = true;

}

void TCP_client::send_random_message() {
    //make the token taken
    delete token_buffer;
    std::string dest = get_random_destination();
    std::string msg = "to nie ma tak ze dobrze czy niedobrze";
    token_buffer = new token(TOK_MSG, id, dest, msg);
    token_buffer->is_free = false;
    want_to_send = false;
    send_token();
    std::cout << "[DEBUG]: Client of id " << id << " has sent message to " << dest << std::endl;
}

std::string TCP_client::get_random_destination() {
    //TODO -> empty case should not happen here (theoretically)
    int index = static_cast<int>( all_peers.empty() ? 0 : rand() % all_peers.size());
    std::cout << "[DEBUG]: Getting random destination from list: " << all_peers.at(index) << std::endl;
    return all_peers.at(index);
}

bool TCP_client::has_peer(std::string peer) {
    for (const auto &all_peer : all_peers) {
        if (all_peer == peer)
            return true;
    }
    return false;
}

void TCP_client::log() {


    std::string log_line = "[" + std::to_string(std::time(0)) + "]: " + id + " has received token" +
                           (token_buffer->type == TOK_INIT ? " TOK_INIT " : " TOK_MSG ") + "with contents: " +
                           token_buffer->get_payload() + " from:" + token_buffer->get_source() + "\n";


    const char* log_line_cstr = log_line.c_str();

    if(sendto(multicast_socket, log_line_cstr,strlen(log_line_cstr)*sizeof(char), 0,
              (const sockaddr *)(&multicast_address), sizeof(multicast_address))<0){
        std::cout<<"[DEBUG]: Sending multicast log went wrong." << std::endl;
        exit(1);
    }
}

void TCP_client::initialize_multicast() {
    //make socket
    if((multicast_socket = init_udp_socket()) == INVALID_SOCKET){
        exit(1);
    }
    multicast_address.sin_family = AF_INET;
    multicast_address.sin_addr.s_addr = inet_addr(multicast_ip.c_str());
    multicast_address.sin_port = htons((uint16_t) multicast_port);

}