//
// Created by Timelock on 13.03.2019.
//

#include <cstring>
#include "Token.hpp"

// We need this kind of method because we need one "frame ( token )" to be of a specific size
void token::set_contents(char* buffer, std::string content){
    strncpy(buffer, content.c_str(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = 0;
}

//When you create token it's free automatically
//You can make token free if you're the destination
token::token(token_type tok_type, std::string source_id, std::string dest_id, std::string data) {
    type = tok_type;
    is_free = true;
    //set destination
    token::set_contents(dest, dest_id);
    //set source
    token::set_contents(source, source_id);
    //set contents
    token::set_contents(payload, data);

    type = tok_type;

}

std::string token::get_payload(){
    return payload;
}

std::string token::get_source(){
    return source;
}

std::string token::get_dest(){
    return dest;
}