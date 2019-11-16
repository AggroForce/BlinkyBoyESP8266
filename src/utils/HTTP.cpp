#include "HTTP.h"

#define __METHOD_MAX_CHARS 7
#define __METHOD_DEF_COUNT 10

const char *__method_str[__METHOD_DEF_COUNT] = {
    "INVALID", //0
    "GET",     //1
    "PUT",     //2
    "HEAD",    //3
    "POST",    //4
    "TRACE",   //5
    "PATCH",   //6
    "DELETE",  //7
    "CONNECT", //8
    "OPTIONS", //9 
};

byte __enum_to_idx(int m, byte lvl){
    if(m & 0b1)
        return lvl+1;
    if(m==0 || lvl >= __METHOD_DEF_COUNT-1)
        return 0;
    return __enum_to_idx(m>>1,lvl+1);
}

HTTPMethod parse_option(const char* s){
    int idx;
    for(idx = 0; idx <= __METHOD_MAX_CHARS;idx++){
        char c = s[idx];
        if(c==' '||c=='\0'){
            break;
        }
    }
    if((idx--)>1){
        switch(s[0]){
            case 'C':
                if(memcmp(s,__method_str[8],sizeof(char)*idx)==0)
                    return HTTPMethod::CONNECT;
                break;
            case 'D':
                if(memcmp(s,__method_str[7],sizeof(char)*idx)==0)
                    return HTTPMethod::DELETE;
                break;
            case 'G':
                if(memcmp(s,__method_str[1],sizeof(char)*idx)==0)
                    return HTTPMethod::GET;
                break;
            case 'H':
                if(memcmp(s,__method_str[3],sizeof(char)*idx)==0)
                    return HTTPMethod::HEAD;
                break;
            case 'O':
                if(memcmp(s,__method_str[9],sizeof(char)*idx)==0)
                    return HTTPMethod::OPTIONS;
                break;
            case 'P':
                if(memcmp(s,__method_str[2],sizeof(char)*idx)==0)
                    return HTTPMethod::PUT;
                if(memcmp(s,__method_str[4],sizeof(char)*idx)==0)
                    return HTTPMethod::POST;
                if(memcmp(s,__method_str[6],sizeof(char)*idx)==0)
                    return HTTPMethod::PATCH;
                break;
            case 'T': 
                if(memcmp(s,__method_str[5],sizeof(char)*idx)==0)
                    return HTTPMethod::TRACE;
                break;
        }
    }  
    return HTTPMethod::INVALID;
}

const char* option_str(HTTPMethod m){
    return __method_str[__enum_to_idx((int)m,0)];
}