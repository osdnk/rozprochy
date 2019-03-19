//
// Created by Timelock on 13.03.2019.
//

#ifndef TOKEN_RING_TOKEN_HPP
#define TOKEN_RING_TOKEN_HPP

#include <iostream>


enum token_type {
    TOK_MSG, TOK_INIT
};

#pragma pack(push)
#pragma pack(1)

class token {
private:
    char source[64] = {};
    char dest[64] = {};
    char payload[1024] = {};
public:

    //TODO - make private fields and getters/setters to make token immutable.
    token_type type;
    bool is_free;


    token(token_type tok_type, std::string source, std::string dest, std::string data);

    void set_contents(char *buffer, std::string content, int len);

    //getters
    std::string get_payload();

    std::string get_source();

    std::string get_dest();

    void set_source(std::string source);

    void set_dest(std::string dest);

    void set_payload(std::string payload);

//    bool is_free();

//    bool set_free(TCP_client *client_instance);
//
//    bool set_destination(TCP_client* client_instance);
//
//    bool set_source(TCP_client* client_instance);
};

#pragma pack(pop)


#endif //TOKEN_RING_TOKEN_HPP
