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
    bool is_free;
public:
    //TODO - declare assignment operator private
    token_type type;


    explicit token(token_type tok_type, std::string source, std::string dest, std::string data);
    void set_contents(char* buffer, std::string content);

    //getters
    std::string get_payload();

    std::string get_source();

    std::string get_dest();

};
#pragma pack(pop)


#endif //TOKEN_RING_TOKEN_HPP
