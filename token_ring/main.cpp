#include <iostream>
#include "TCP_client.hpp"

int main(int argc, char **argv) {


    //I assume that user's not an idiot.
    //argv1 -> My listening port
    //argv2 -> Neighbor port
    //argv3 -> Client's id
    int my_port, neighbor_port;
    sscanf(argv[1], "%d", &my_port);
    sscanf(argv[2], "%d", &neighbor_port);
    std::string client_id = argv[3];
    srand((time(NULL)));
    TCP_client *new_client = new TCP_client(my_port, neighbor_port, client_id);
}