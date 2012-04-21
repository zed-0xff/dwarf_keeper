#include <microhttpd.h>
#include "common.h"
#ifndef HTTP_REQUEST_CPP
#define HTTP_REQUEST_CPP

class HTTPRequest {
    struct MHD_Connection*conn;
    const char*url;

    public:

    HTTPRequest(struct MHD_Connection* conn, const char* url){
        this->conn = conn;
        this->url  = url;
    }

    int get_int(const char*param_name, int default_value){
        pair<const char*, int> ip(param_name,default_value);
        MHD_get_connection_values(conn, MHD_GET_ARGUMENT_KIND, &http_int_param_iter, &ip);
        return ip.second;
    }

    bool url_match(const char*url){
        return !strcmp(this->url, url);
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    private:

    static int http_int_param_iter(void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
        pair<const char*, int> *p = (pair<const char*, int>*)cls;
        if(!strcmp(p->first, key)){
            p->second = atoi(value);
            return MHD_NO;
        } else {
            return MHD_YES; // continue iteration
        }
    }

};

#endif
