#include <Arduino.h>

#ifndef _h_http
#define _h_http

enum HTTPMethod{
    GET     =0b0,
    PUT     =0b1,
    HEAD    =0b10,
    POST    =0b100,
    TRACE   =0b1000,
    PATCH   =0b10000,
    DELETE  =0b100000,
    CONNECT =0b1000000,
    OPTIONS =0b10000000,
    INVALID =0b100000000
};

HTTPMethod parse_option(const char* s);
const char* option_str(HTTPMethod m);

#endif
