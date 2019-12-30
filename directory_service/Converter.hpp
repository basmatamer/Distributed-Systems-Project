//
//  Converter.hpp
//  Distributed Project
//
//  Created by Mohamed on 10/27/18.
//  Copyright © 2018 Mohamed Tawfik. All rights reserved.
//

#ifndef Converter_hpp
#define Converter_hpp

#include <string>

class Converter {
public:
    static void int_to_chars(uint32_t integer, char* chars);
    static uint32_t chars_to_int(char* chars);
    
    static void short_to_chars(uint16_t short_int, char* chars);
    static uint16_t chars_to_short(char* chars);
    
    static void bool_to_chars(bool value, char* chars);
    static bool chars_to_bool(char* chars);
    
    static void string_to_chars(const std::string& str, char* chars);
    static std::string chars_to_string(uint32_t str_length, char* chars);
};
#endif /* Converter_hpp */
