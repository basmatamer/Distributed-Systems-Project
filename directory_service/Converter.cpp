//
//  Converter.cpp
//  Distributed Project
//
//  Created by Mohamed on 10/27/18.
//  Copyright © 2018 Mohamed Tawfik. All rights reserved.
//

#include "Converter.hpp"
#include <string>
#include <iostream>
#include <arpa/inet.h>

void Converter::int_to_chars(uint32_t integer, char* chars) {
    integer = htonl(integer);
   // std::cout << "Attempting to convert int: 0x" << std::hex << integer << std::endl;

    for(int i = 0; i < 4; i++) {
        chars[i] = (integer >> (i * 8)) & 255;
    }
}
unsigned int Converter::chars_to_int(char* chars) {
    uint32_t result = 0;
    for(int i = 0; i < 4; i++) {
        result = result | ( ( (uint32_t)(unsigned char)(chars[i]) ) << (i * 8) );
    }
   // std::cout << "This int was just converted: 0x" << std::hex << result << std::endl;
    result = ntohl(result);
    return result;
}

void Converter::string_to_chars(const std::string& str, char* chars) {
    for(unsigned int i = 0; i < str.length(); i++) {
        chars[i] = str[i];
    }
}
std::string Converter::chars_to_string(uint32_t str_length, char* chars) {
    std::string str = "";
    for(unsigned int i = 0; i < str_length; i++) {
        str += chars[i];
    }
    return str;
}

void Converter::short_to_chars(uint16_t short_int, char* chars) {
    short_int = htons(short_int);
//    std::cout << "Attempting to convert int: 0x" << std::hex << short_int << std::endl;

    for(int i = 0; i < 2; i++) {
        chars[i] = (short_int >> (i * 8)) & 255;
    }
}
uint16_t Converter::chars_to_short(char* chars) {
    uint16_t result = 0;
    for(int i = 0; i < 2; i++) {
        result = result | ( ( (uint16_t)(unsigned char)(chars[i]) ) << (i * 8) );
    }
   // std::cout << "This short was just converted: 0x" << std::hex << result << std::endl;
    result = ntohs(result);
    return result;
}

void Converter::bool_to_chars(bool value, char* chars) {
    chars[0] = (value) ? 1 : 0;
}
bool Converter::chars_to_bool(char* chars) {
    return (chars[0] == 1);
}
