#ifndef FETCHER_CPP
#define FETCHER_CPP

#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>
#include <string>
#include <stdint.h>
#include <SDL/SDL.h>
#include <snappy.h>

using namespace std;

class Fetcher {
    private:

    CURL *curl_handle;
    curl_slist *headers;

    public:

    string data;
    int debug;

    const char*host;
    int port;

    uint64_t total_dl;

    char errbuf[CURL_ERROR_SIZE];

    static const char* g_host;

    Fetcher(){
        debug = 0;
        host = g_host ? g_host : "localhost";
        port = 4545;

        total_dl = 0;
        headers = NULL;

        memset(errbuf,0,sizeof(errbuf));

        curl_handle = curl_easy_init();

        curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS,     1L);
        curl_easy_setopt(curl_handle, CURLOPT_TCP_NODELAY,    1L);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 0L);

        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_func);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, this);

        curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, header_func);
        curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, this);

        curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, errbuf);
    }

    void add_header(const char* hdr){
        headers = curl_slist_append(headers, hdr);
        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
    }

    void fetch_url(const char* url_tail){
        char url[0x1000];
        sprintf(url, "http://%s:%d%s", host, port, url_tail);

        data.clear();

        if( debug ) curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L); 

        curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_URL,     url);

        if( 0 != curl_easy_perform(curl_handle)){
            printf("[!] %s\n", errbuf);
            sleep(1);
        }

        if( debug ) printf("[d] %s: got %ld bytes\n", __func__, data.size() );

        double dd = 0; long dh = 0;
        curl_easy_getinfo(curl_handle, CURLINFO_SIZE_DOWNLOAD, &dd);
        curl_easy_getinfo(curl_handle, CURLINFO_HEADER_SIZE, &dh);
        dh += dd;
        total_dl += dh;
    }

    size_t fetch_screen(RemoteScreen& scr){
        char url[0x40];
        sprintf(url,"/live.bin?h=0x%x", scr.hash);
        fetch_url(url);

        if( data.size() > 8 ){
            string output;
            snappy::Uncompress(data.data(), data.size(), &output);
            //printf("[d] uncompress: %d -> %d\n", data.size(), output.size());

            uint16_t*p = (uint16_t*)output.data();
            scr.width  = p[0];
            scr.height = p[1];
            scr.setData(&p[2]);
            return data.size();
        } else {
            return 0;
        }
    }

    string* fetch_tile(uint32_t tile_id){
        char url[0x100];
        sprintf(url, "/live/tile.snp?id=0x%x", tile_id);
        fetch_url(url);
        if(data.size() > 6){
            string output;
            snappy::Uncompress(data.data(), data.size(), &output);
            data = output;
        }
        return &data;
    }

    void post_events(vector<SDL_Event> &v){
        string url = "/live/sdl_events";
        char buf[0x200];
        uint32_t *p;
        int i,j,n;

        sprintf(buf, "?n=%ld", v.size()); url += buf;

        for(i=0; i<v.size(); i++){
            p = (uint32_t*)&v[i];
            n = sizeof(SDL_Event)/4;

            while(n>0 && p[n-1] == 0) n--; // do not send tail zero dwords

            for(j=0; j<n; j++){
                if( p[j] < 10 ){
                    // decimal
                    sprintf(buf, "&e%d=%d", i, p[j]);
                } else {
                    sprintf(buf, "&e%d=0x%x", i, p[j]);
                }
                url += buf;
            }
        }
        fetch_url(url.c_str());
    }

    ~Fetcher(){
        curl_easy_cleanup(curl_handle);
    }

    private:

    static size_t write_func(void *ptr, size_t size, size_t nmemb, void *userdata) {
        Fetcher* fetcher = (Fetcher*)userdata;
        size_t chunk_size = size*nmemb;

        if( fetcher->debug > 1 ) printf("[d] %s: got %ld bytes\n", __func__, chunk_size );

        fetcher->data.append((const char*)ptr, chunk_size);

        return chunk_size;
    }

    static size_t header_func(void *ptr, size_t size, size_t nmemb, void *userdata) {
        Fetcher* fetcher = (Fetcher*)userdata;
        size_t chunk_size = size*nmemb;

        if( fetcher->debug > 1 ) printf("[d] %s: got %ld bytes\n", __func__, chunk_size );

        static const char len_hdr[] = "Content-Length: ";
        if( !strncmp((char*)ptr, len_hdr, strlen(len_hdr))){
            size_t data_size = strtoul((char*)ptr + strlen(len_hdr), NULL, 10);
            fetcher->data.reserve( data_size );
        }

        return chunk_size;
    }
};

const char* Fetcher::g_host = NULL;

#endif
