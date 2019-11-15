#include "HTTP.h"

#define __METHOD_MAX_CHARS 7
#define __METHOD_DEF_COUNT 10

const char* __method_str[__METHOD_DEF_COUNT] = {
    "GET",     //0
    "PUT",     //1
    "HEAD",    //2
    "POST",    //3
    "TRACE",   //4
    "PATCH",   //5
    "DELETE",  //6
    "CONNECT", //7
    "OPTIONS", //8
    "INVALID"  //9
};

byte __enum_to_idx(int m, byte lvl){
    if(m & 0b1 || lvl >= __METHOD_DEF_COUNT-1)
        return lvl;
    return __enum_to_idx(m>>1,lvl+1);
}

HTTPMethod parse_option(const char* s){
    int idx;
    for(idx = 0; idx <= __METHOD_MAX_CHARS; idx++){
        char c = s[idx];
        if(c==' '||c=='\0'){
            break;
        }
    }
    if(idx>1){
        switch(s[0]){
            case 'C':
                if(memcmp(s,__method_str[7],sizeof(char)*idx))
                    return HTTPMethod::CONNECT;
            case 'D':
                if(memcmp(s,__method_str[6],sizeof(char)*idx))
                    return HTTPMethod::DELETE;
            case 'G':
                if(memcmp(s,__method_str[0],sizeof(char)*idx))
                    return HTTPMethod::GET;
            case 'H':
                if(memcmp(s,__method_str[2],sizeof(char)*idx))
                    return HTTPMethod::HEAD;
            case 'O':
                if(memcmp(s,__method_str[8],sizeof(char)*idx))
                    return HTTPMethod::OPTIONS;
            case 'P':
                if(memcmp(s,__method_str[1],sizeof(char)*idx))
                    return HTTPMethod::PUT;
                if(memcmp(s,__method_str[3],sizeof(char)*idx))
                    return HTTPMethod::POST;
                if(memcmp(s,__method_str[5],sizeof(char)*idx))
                    return HTTPMethod::PATCH;
            case 'T': 
                if(memcmp(s,__method_str[4],sizeof(char)*idx))
                    return HTTPMethod::TRACE;
        }
    }  
    return HTTPMethod::INVALID;
}

const char* option_str(HTTPMethod m){
    return __method_str[__enum_to_idx((int)m,0)];
}