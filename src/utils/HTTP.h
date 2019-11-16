#include <Arduino.h>

#ifndef _h_http
#define _h_http

enum HTTPMethod{
    GET     =0b1,
    PUT     =0b10,
    HEAD    =0b100,
    POST    =0b1000,
    TRACE   =0b10000,
    PATCH   =0b100000,
    DELETE  =0b1000000,
    CONNECT =0b10000000,
    OPTIONS =0b100000000,
    INVALID =0b0
};


HTTPMethod parse_option(const char* s);
const char* option_str(HTTPMethod m);

class HTTPRequest{
    public:
        HTTPMethod method;
        String body;
};

#endif
