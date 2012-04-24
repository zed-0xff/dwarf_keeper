#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>

#include <microhttpd.h>

//#define DEBUG

#include "common.h"
#include "clothes.cpp"
#include "items_controller.cpp"
#include "creatures_controller.cpp"

//#define USE_FORK

static pid_t(*orig_getpid)() = NULL;

static struct MHD_Daemon* mhd = NULL;

void dostuff(int);
void memserver_start();
void memserver_accept();

pid_t getpid(){
    if(!mhd){
        orig_getpid = (pid_t (*)()) dlsym(RTLD_NEXT, "getpid");
        printf("[.] (pid=%d) starting server...\n",orig_getpid());
        memserver_start();
    }
    memserver_accept();
    return orig_getpid();
}

///////////////////////////////////////////////////////////////////////////////////

struct hexdump_params{
    unsigned long offset;
    unsigned long size;
    int width;
};

int get_hexdump_params(void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
    struct hexdump_params* hp = (struct hexdump_params*)cls;
    if(!strcmp(key, "offset")){
        hp->offset = strtoul(value, NULL, 0);
    } else if(!strcmp(key, "size")){
        hp->size = strtoul(value, NULL, 0);
    } else if(!strcmp(key, "width")){
        hp->width= atoi(value);
    }
    return MHD_YES;
}

typedef pair<const char*, int> int_param_t;
int http_int_param_iter(void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
    int_param_t*p = (int_param_t*)cls;
    if(!strcmp(p->first, key)){
        p->second = atoi(value);
        return MHD_NO;
    } else {
        return MHD_YES; // continue iteration
    }
}
int http_get_int(struct MHD_Connection* conn, const char*param_name, int default_value){
    int_param_t ip(param_name,default_value);
    MHD_get_connection_values(conn, MHD_GET_ARGUMENT_KIND, &http_int_param_iter, &ip);
    return ip.second;
}


typedef pair<const char*, string> str_param_t;
int http_str_param_iter(void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
    str_param_t*p = (str_param_t*)cls;
    if(!strcmp(p->first, key)){
        p->second = value;
        return MHD_NO;
    } else {
        return MHD_YES; // continue iteration
    }
}
string http_get_string(struct MHD_Connection* conn, const char*param_name, const char* default_value){
    str_param_t sp(param_name,default_value);
    MHD_get_connection_values(conn, MHD_GET_ARGUMENT_KIND, &http_str_param_iter, &sp);
    return sp.second;
}


typedef pair<const char*, vector<string>* > strs_param_t;
int http_strs_param_iter(void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
    strs_param_t*p = (strs_param_t*)cls;
    if(!strcmp(p->first, key)){
        p->second->push_back(value);
        return MHD_YES; // continue iteration
    }
}
vector<string> http_get_strings(struct MHD_Connection* conn, const char*param_name){
    static vector<string>r;
    r.clear();
    strs_param_t sp(param_name, &r);
    MHD_get_connection_values(conn, MHD_GET_ARGUMENT_KIND, &http_strs_param_iter, &sp);
    return r;
}

bool is_ajax(struct MHD_Connection* conn){
    str_param_t sp("X-Requested-With","");
    MHD_get_connection_values(conn, MHD_HEADER_KIND, &http_str_param_iter, &sp);
    return sp.second == "XMLHttpRequest";
}

///////////////////////////////////////////////////////////////////////////////////

static int ahc_echo(void * cls,
                    struct MHD_Connection * conn,
                    const char * url,
                    const char * method,
                    const char * version,
                    const char * upload_data,
                    size_t * upload_data_size,
                    void ** ptr) {
  static int dummy;
  struct MHD_Response * response;
  int ret;
  char buf[0x1000];
  string html,s;
  int is_json = 0;
  int resp_code = MHD_HTTP_OK;

  HTTPRequest request(conn, url);

  if (0 != strcmp(method, "GET")) return MHD_NO; /* unexpected method */
  if (&dummy != *ptr){
      /* The first time only the headers are valid,
         do not respond in the first round... */
      *ptr = &dummy;
      return MHD_YES;
    }

  if (0 != *upload_data_size) return MHD_NO; /* upload data in a GET!? */
  *ptr = NULL; /* clear context pointer */

  html.reserve(50*1024);

  if(!strcmp(url, "/dwarves") || !strcmp(url, "/creatures")){
      CreaturesController c(request);
      html += c.to_html();
      resp_code = c.resp_code;

  } else if(!strcmp(url, "/clothes")){
      Clothes clothes;

      clothes.want_owned    = http_get_int(conn, "owned", 1);
      clothes.want_free     = http_get_int(conn, "free", 1);
      clothes.want_unusable = http_get_int(conn, "unusable", 0);
      clothes.free_max_wear = http_get_int(conn, "free_max_wear", Item::WEAR_OK);
      clothes.want_stats    = !is_ajax(conn);

      clothes.setWantTypes(http_get_strings(conn, "t"));

      html.reserve(100*1024);
      html += clothes.to_html();

  } else if(!strcmp(url, "/items")){
      ItemsController c(request);
      html += c.to_html();

  } else if(!strcmp(url, "/dwarves.json")){
      is_json = 1;
      html = "[";
      int idx = 0;
      int was_first = 0;
      while(Dwarf* pDwarf=Dwarf::getNext(&idx)){
          s = cp850_to_json(pDwarf->getName());
          sprintf(buf, "%s{\"id\":%d,\"name\":\"%s\"}", was_first?",":"", pDwarf->getId(), s.c_str());
          html += buf;
          was_first = 1;
      }
      html += "]";
  } else if(!strcmp(url, "/style.css")){
      int fd;
      struct stat sbuf;
      if ( (-1 == (fd = open ("dwarf_keeper/style.css", O_RDONLY))) || (0 != fstat (fd, &sbuf)) ){
          sprintf(buf, "[!] %s: ", url);
          perror(buf);
          if (fd != -1) close (fd);
          return MHD_NO;
      }
      response = MHD_create_response_from_fd_at_offset (sbuf.st_size, fd, 0);
      MHD_add_response_header (response, "Content-Type", "text/css");
      ret = MHD_queue_response (conn, MHD_HTTP_OK, response);
      MHD_destroy_response (response);
      return ret;
  } else if(!strcmp(url+strlen(url)-3,".js") && !strstr(url,"..") && !strchr(url+1,'/') && !strchr(url,'\\')){
      sprintf(buf,"dwarf_keeper%s",url);
      int fd;
      struct stat sbuf;
      if ( (-1 == (fd = open (buf, O_RDONLY))) || (0 != fstat (fd, &sbuf)) ){
          sprintf(buf, "[!] %s: ", url);
          perror(buf);
          if (fd != -1) close (fd);
          return MHD_NO;
      }
      response = MHD_create_response_from_fd_at_offset (sbuf.st_size, fd, 0);
      MHD_add_response_header (response, "Content-Type", "application/x-javascript");
      ret = MHD_queue_response (conn, MHD_HTTP_OK, response);
      MHD_destroy_response (response);
      return ret;
  } else if(!strcmp(url,"/hexdump")){
      struct hexdump_params hp; hp.size = hp.offset = 0; hp.width = 1;
      MHD_get_connection_values(conn, MHD_GET_ARGUMENT_KIND, &get_hexdump_params, &hp);

      string title = http_get_string(conn,"title","");
      if(!title.empty()){
          html += "<title>" + html_escape(title) + "</title>\n";
          html += "<h2>" + html_escape(title) + "</h2>\n";
      }

      html += "<pre>";
      switch(hp.width){
          case 4:{
                  uint32_t *p = (uint32_t*)hp.offset;
                  for(long i=0; i<hp.size; i+=4, p++){
                      if(i%0x10 == 0){ sprintf(buf, "\n%p ", p); html += buf; }
                      sprintf(buf, " %08x", *p); 
                      html += buf;
                  }
                 }
              break;
          default:{
                  unsigned char*p = (unsigned char*)hp.offset;
                  for(long i=0; i<hp.size; i++, p++){
                      if(i%0x10 == 0){ sprintf(buf, "\n%p ", p); html += buf; }
                      sprintf(buf, " %02x", *p); 
                      html += buf;
                  }
                  }
              break;
      }
      html += "</pre>";
  } else return MHD_NO;

  string html_utf8;
  if(is_json){
      html_utf8 = html;
  } else if(is_ajax(conn)){
      html_utf8 = cp850_to_utf8(html);
  } else {
      html_utf8 = 
          "<html><head>\n"
          "\t<link href='/style.css' rel='stylesheet' type='text/css' />\n"
          "</head><body>\n" +
          cp850_to_utf8(html) + "\n\n" +
          "<script src='/jquery-1.7.2.min.js'></script>\n"
          "<script src='/sorttable.js'></script>\n"
          "<script src='/dwarf_keeper.js'></script>\n"
          "</body></html>";
  }

  response = MHD_create_response_from_buffer(html_utf8.size(), (void*) html_utf8.data(), MHD_RESPMEM_MUST_COPY);
  if(is_json){
      MHD_add_response_header(response, "Content-Type", "application/json");
  } else {
      MHD_add_response_header(response, "Content-Type", "text/html; charset=utf-8");
  }
  ret = MHD_queue_response(conn, resp_code, response);
  MHD_destroy_response(response);

  return ret;
}

void memserver_start(){
  mhd = MHD_start_daemon(0,
                       4545,
                       NULL, NULL,      // accept policy  callback + argument
                       &ahc_echo, NULL, // access handler callback + argument
                       MHD_OPTION_END);
  if (mhd == NULL){
    perror("[!] failed to start HTTP server: ");
    exit(1);
  }
}

void memserver_accept(){
      fd_set sr,sw,sx;
      int max_fd = 0;
      struct timeval tv;
      unsigned MHD_LONG_LONG mhd_timeout;

      FD_ZERO(&sr); FD_ZERO(&sw); FD_ZERO(&sx);

      if( MHD_YES != MHD_get_fdset(mhd,&sr,&sw,&sx,&max_fd)){
          perror("[!] MHD_fet_fdset failed: ");
          exit(1);
      }

      tv.tv_sec = 0;
      tv.tv_usec = 1000;

      select (max_fd + 1, &sr, &sw, &sx, &tv);
      MHD_run(mhd);
}
