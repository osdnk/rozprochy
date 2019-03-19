//
// Created by Timelock on 13.03.2019.
//

#include <cstring>
#include "Token.hpp"

// We need this kind of method because we need one "frame ( token )" to be of a specific size
void token::set_contents(char *buffer, std::string content, int len) {
    strncpy(buffer, content.c_str(), len);
}

//When you create token it's free automatically
//You can make token free if you're the destination
token::token(token_type tok_type, std::string source_id, std::string dest_id, std::string data) {
    type = tok_type;
    is_free = true;

    set_source(source_id);
    set_dest(dest_id);
    set_payload(data);

    type = tok_type;

}

std::string token::get_payload() {
    return std::string(payload);
}

std::string token::get_source() {
    return std::string(source);
}

std::string token::get_dest() {
    return std::string(dest);
}

void token::set_source(std::string source_id){
    //set source
    set_contents(source, source_id, 64);
}

void token::set_dest(std::string dest_str){
    //set source
    set_contents(dest, dest_str, 64);
}

void token::set_payload(std::string payload_str){
    //set source
    set_contents(payload, payload_str, 1024);
}