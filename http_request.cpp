#include <microhttpd.h>
#include "common.h"
#ifndef HTTP_REQUEST_CPP
#define HTTP_REQUEST_CPP


class HTTPRequest {
    struct MHD_Connection*conn;
    const char*url;

    typedef pair<const char*, string> str_param_t;
    typedef pair<const char*, vector<string>* > strs_param_t;

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

    vector<int> get_ints(const char*param_name){
        vector<int>r;
        pair<const char*, vector<int>*> ip(param_name,&r);
        MHD_get_connection_values(conn, MHD_GET_ARGUMENT_KIND, &http_ints_param_iter, &ip);
        return r;
    }

    uint32_t get_uint(const char*param_name, uint32_t default_value){
        pair<const char*, uint32_t> ip(param_name,default_value);
        MHD_get_connection_values(conn, MHD_GET_ARGUMENT_KIND, &http_uint_param_iter, &ip);
        return ip.second;
    }

    vector<string> get_strings(const char*param_name){
        vector<string>r;
        strs_param_t sp(param_name, &r);
        MHD_get_connection_values(conn, MHD_GET_ARGUMENT_KIND, &http_strs_param_iter, &sp);
        return r;
    }

    string get_string(const char*param_name, const char* default_value){
        str_param_t sp(param_name,default_value);
        MHD_get_connection_values(conn, MHD_GET_ARGUMENT_KIND, &http_str_param_iter, &sp);
        return sp.second;
    }

    bool is_ajax(){
        str_param_t sp("X-Requested-With","");
        MHD_get_connection_values(conn, MHD_HEADER_KIND, &http_str_param_iter, &sp);
        return sp.second == "XMLHttpRequest";
    }

    bool url_match(const char*url){
        return !strcmp(this->url, url);
    }

    ////////////////////////////////////////////////////////////////////////////////////////

    private:

    // stop on first found
    static int http_int_param_iter(void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
        pair<const char*, int> *p = (pair<const char*, int>*)cls;
        if(!strcmp(p->first, key)){
            p->second = strtol(value, NULL, 0);
            return MHD_NO;
        } else {
            return MHD_YES; // continue iteration
        }
    }

    // return all params as a vector
    static int http_ints_param_iter(void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
        pair<const char*, vector<int>*> *p = (pair<const char*, vector<int>*>*)cls;
        if(!strcmp(p->first, key)){
            p->second->push_back(strtol(value,NULL,0));
        }
        return MHD_YES; // continue iteration
    }

    static int http_uint_param_iter(void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
        pair<const char*, uint32_t> *p = (pair<const char*, uint32_t>*)cls;
        if(!strcmp(p->first, key)){
            p->second = strtoul(value, NULL, 0);
            return MHD_NO;
        } else {
            return MHD_YES; // continue iteration
        }
    }


    static int http_str_param_iter(void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
        str_param_t*p = (str_param_t*)cls;
        if(!strcmp(p->first, key)){
            p->second = value;
            return MHD_NO;
        } else {
            return MHD_YES; // continue iteration
        }
    }

    static int http_strs_param_iter(void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
        strs_param_t*p = (strs_param_t*)cls;
        if(!strcmp(p->first, key)){
            p->second->push_back(value);
            return MHD_YES; // continue iteration
        }
    }

};

#endif
