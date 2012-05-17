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
#include <SDL/SDL.h>

//#define DEBUG

#include "common.h"
#include "clothes_controller.cpp"
#include "items_controller.cpp"
#include "units_controller.cpp"
#include "trade_controller.cpp"
#include "buildings_controller.cpp"
#include "screen_controller.cpp"
#include "live_controller.cpp"

static const char* ALLOWED_CONTENT_TYPES[][2] = {
    {".js",  "application/x-javascript"},
    {".css", "text/css"},
    {".gif", "image/gif"},
    {".png", "image/png"},
    {".ico", "image/png"},
    {".html","text/html"}
};

static pid_t(*orig_getpid)() = NULL;

static struct MHD_Daemon* mhd = NULL;
static bool hooks_set_up = false;

void dostuff(int);
void memserver_start();
void memserver_accept();

int  SDLCALL (*orig_SDL_PollEvent)(SDL_Event *event) = NULL;
int  SDLCALL SDL_PollEvent(SDL_Event*ev);

void SDLCALL (*orig_SDL_Delay)(Uint32) = NULL;
void SDLCALL SDL_Delay(Uint32);

int  SDLCALL (*orig_SDL_NumJoysticks)() = NULL;
int  SDLCALL SDL_NumJoysticks();

static int n_getpid=0, n_delay=0, n_poll_event=0, n_num_joy=0;

void* _hook_sdl_func(const char*func_name, void*local_func){
    // try fastest method first
    void *p = dlsym(RTLD_NEXT, func_name);
    if( p && p != local_func){
        return p;
    }

    // a slower one
    p = dlsym(RTLD_DEFAULT, func_name);
    if( p && p != local_func){
        return p;
    }

    // OSX
    const char* sdl_lib_path = "@executable_path/../Frameworks/SDL.framework/Versions/A/SDL";

    void *plib  = dlopen(sdl_lib_path, RTLD_LAZY);
    void *pfunc = NULL;

    if(plib){
        pfunc = dlsym(plib, func_name);
        dlclose(plib);
    } else {
        fprintf(stderr, "[!] Error: cannot dlopen %s\n", sdl_lib_path);
    }

    if(!pfunc){
        fprintf(stderr, "[!] Error: cannot hook %s!\n", func_name);
    }

    return pfunc;
}

void setup_hooks(){
    hooks_set_up = true;
    void *p;

    orig_getpid = (pid_t (*)()) dlsym(RTLD_NEXT, "getpid");
    if( !orig_getpid ){
        orig_getpid = NULL;
        fprintf(stderr, "[!] Error: cannot hook getpid!\n");
    } else {
        printf("[.] pid=%d\n", orig_getpid());
    }

    os_init();

    *(void**)&orig_SDL_PollEvent    = _hook_sdl_func("SDL_PollEvent",    (void*)SDL_PollEvent);
    *(void**)&orig_SDL_Delay        = _hook_sdl_func("SDL_Delay",        (void*)SDL_Delay);
    *(void**)&orig_SDL_NumJoysticks = _hook_sdl_func("SDL_NumJoysticks", (void*)SDL_NumJoysticks);

    if( !orig_SDL_PollEvent ){
        fprintf(stderr, "[!] Continuing without keyboard and mouse access... :(\n");
    }
}

int SDLCALL SDL_NumJoysticks(){
    n_num_joy++;

    if(!hooks_set_up){
        setup_hooks();
        memserver_start();
    }

    LiveController::copy_screen();
    memserver_accept();

    if(orig_SDL_NumJoysticks){
        return orig_SDL_NumJoysticks();
    } else {
        return 0;
    }
}

void SDLCALL SDL_Delay(Uint32 ms){
    struct timeval t0, t1;
    gettimeofday(&t0, NULL);

    n_delay++;

    if(!hooks_set_up){
        setup_hooks();
        memserver_start();
    }

    memserver_accept();

    if(orig_SDL_Delay){
        gettimeofday(&t1, NULL);
        Uint32 ms1 = ms-diff_ms(t1,t0);
        // use min() to avoid overflow on signed/unsigned conversion
        orig_SDL_Delay(min(ms1,ms)); 
    } else {
        fprintf(stderr, "[!] orig_SDL_Delay = NULL, cannot continue!\n");
        exit(1);
    }
}

int SDLCALL SDL_PollEvent(SDL_Event*ev){
    n_poll_event++;

    if(!hooks_set_up){
        setup_hooks();
        memserver_start();
    }

    if( !g_override_keys.empty() ){
        memcpy(ev, &g_override_keys.front(), sizeof(*ev));
        g_override_keys.pop();
        return 1;
    }

    memserver_accept();

    if( !g_override_keys.empty() ){
        memcpy(ev, &g_override_keys.front(), sizeof(*ev));
        g_override_keys.pop();
        return 1;
    }

    if(orig_SDL_PollEvent){
        return orig_SDL_PollEvent(ev);
    } else {
        return 0;
    }
}


pid_t getpid(){
    n_getpid++;

    if(!hooks_set_up){
        setup_hooks();
        memserver_start();
    }

    memserver_accept();

    if( orig_getpid ){
        return orig_getpid();
    } else {
        return -1;
    }
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

///////////////////////////////////////////////////////////////////////////////////

static int process_request(void * cls,
                    struct MHD_Connection * conn,
                    const char * url,
                    const char * method,
                    const char * version,
                    const char * upload_data,
                    size_t * upload_data_size,
                    void ** ptr) {
  static int dummy;
  struct MHD_Response * response = NULL;
  int ret;
  char buf[0x1000];
  string html,s;
  int is_json = 0;
  int resp_code = MHD_HTTP_OK;

  HTTPRequest request(conn, url);

  if (0 != strcmp(method, "GET")){
      printf("[!] unexpected method: %s %s\n", method, url);
      return MHD_NO; /* unexpected method */
  }

  if (&dummy != *ptr){
      /* The first time only the headers are valid,
         do not respond in the first round... */
      *ptr = &dummy;
      return MHD_YES;
    }

  if (0 != *upload_data_size) return MHD_NO; /* upload data in a GET!? */
  *ptr = NULL; /* clear context pointer */

  html.reserve(50*1024);

  if(request.url_starts_with("/dwarves") || request.url_starts_with("/units")){
      UnitsController c(request);
      html += c.to_html();
      resp_code = c.resp_code;

  } else if(request.url_match("/clothes")){
      ClothesController c(request);
      html.reserve(100*1024);
      html += c.to_html();

  } else if(request.url_match("/items")){
      ItemsController c(request);
      html += c.to_html();
      resp_code = c.resp_code;

  } else if(request.url_match("/buildings")){
      BuildingsController c(request);
      html += c.to_html();
      resp_code = c.resp_code;

  } else if(!strcmp(url, "/trade")){
      TradeController c(request);
      html += c.to_html();
      resp_code = c.resp_code;

  } else if(request.url_starts_with("/screen")){
      ScreenController c(request);
      html += c.to_html();
      resp_code = c.resp_code;

  } else if(request.url_starts_with("/live")){
      LiveController c(request);
      html += c.to_html();
      resp_code = c.resp_code;
      if( c.response ) response = c.response;

  } else if(!strcmp(url,"/hexdump")){
      struct hexdump_params hp;
      hp.offset = (uint32_t)&hp; // stack top
      hp.size   = 0x100;
      hp.width  = 4;

      MHD_get_connection_values(conn, MHD_GET_ARGUMENT_KIND, &get_hexdump_params, &hp);
      html += HTML::hexdump((void*)hp.offset, hp.size, hp.width, 
              request.get_string("title", (hp.offset == (uint32_t)&hp) ? "(stack top)" : "")
              );
  } else if(request.url_starts_with("/stat")){
      sprintf(buf, 
              "<table class=t1>\n"
                "<tr>"
                    "<th> n_getpid"
                    "<td class=r> %d\n"
                "<tr>"
                    "<th> n_poll_event"
                    "<td class=r> %d\n"
                "<tr>"
                    "<th> n_delay"
                    "<td class=r> %d\n"
                "<tr>"
                    "<th> n_num_joy"
                    "<td class=r> %d\n"
              "</table>\n",
              n_getpid,
              n_poll_event,
              n_delay,
              n_num_joy
             );
      html += buf;

  } else {
      // security
      if(strstr(url, "..") || strstr(url,"//") || strchr(url,'\\')) return MHD_NO;

      //printf("[.] %s %s\n", method, url);

      int lUrl = strlen(url);
      for(int i=0; i<sizeof(ALLOWED_CONTENT_TYPES)/sizeof(ALLOWED_CONTENT_TYPES[0]); i++){
          const char* ext   = ALLOWED_CONTENT_TYPES[i][0];
          const char* ctype = ALLOWED_CONTENT_TYPES[i][1];
          int lExt = strlen(ext);

          if(lUrl > lExt && !strcmp(url+lUrl-lExt,ext)){
              sprintf(buf,"dwarf_keeper%s",url);
              int fd;
              struct stat sbuf;
              if ( (-1 == (fd = open (buf, O_RDONLY))) || (0 != fstat (fd, &sbuf)) ){
                  sprintf(buf, "[!] %s: ", url);
                  perror(buf);
                  if (fd != -1) close (fd);
                  // file not found or cannot open
                  return MHD_NO;
              }
              response = MHD_create_response_from_fd_at_offset (sbuf.st_size, fd, 0);
              MHD_add_response_header(response, "Content-Type", ctype);

              if( strstr(url, ".min.js") || strstr(ctype, "image/") ){
                  // support client-side caching
                  char buf[0x200];
                  time_t t = time(NULL);
                  t += 24*60*60; // cache for 24 hours
                  strftime(buf,sizeof(buf),"%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
                  MHD_add_response_header (response, "Expires", buf);
                  MHD_add_response_header (response, "Cache-Control", "public");
              }

              ret = MHD_queue_response (conn, MHD_HTTP_OK, response);
              MHD_destroy_response (response);
              return ret;
          }
      }

      // not allowed file extension or unknown url
      resp_code = MHD_HTTP_NOT_FOUND;
      html += "<div class=error>Unknown URL</div>";
  }
      
  if(is_json || request.is_ajax() || html == "OK"){
      // no templating
  } else {
      int fd;
      struct stat sbuf;
      size_t size = 0;
      string tpl;

      if ( (-1 == (fd = open ("dwarf_keeper/template.html", O_RDONLY))) || (0 != fstat (fd, &sbuf)) ){
          perror("[!] template.html: ");
          if (fd != -1) close (fd);
      } else {
          tpl.resize(sbuf.st_size); // fills with NULL chars by default
          read(fd, &tpl[0], sbuf.st_size);
          close(fd);
      }

      if( str_replace(tpl, "{CONTENT}", html) == 0 ){
          // assign default template here b/c template read from file might be invalid
          tpl =
              "<html><head>\n"
              "\t<link href='/style.css' rel='stylesheet' type='text/css' />\n"
              "</head><body>\n" +
              html + "\n\n" +
              "<script src='/jquery-1.7.2.min.js'></script>\n"
              "<script src='/sorttable.js'></script>\n"
              "<script src='/dwarf_keeper.js'></script>\n"
              "<font color=red>"
                  " no {CONTENT} tag in template.html, or template.html not found. Using built-in default"
              "</font>\n"
              "</body></html>";
      }

      // highlight text on page, from 'hl' url param

      string hl = request.get_string("hl","");
      if( !hl.empty() ){
          str_replace(tpl, hl, "<span class=hl>" + hl + "</span>");
      }
      html = tpl;
  }

  if( !response ){
      response = MHD_create_response_from_buffer(html.size(), (void*) html.data(), MHD_RESPMEM_MUST_COPY);
      if(is_json){
          MHD_add_response_header(response, "Content-Type", "application/json");
      } else {
          MHD_add_response_header(response, "Content-Type", "text/html; charset=utf-8");
      }
  }
  ret = MHD_queue_response(conn, resp_code, response);
  MHD_destroy_response(response);

  return ret;
}

void memserver_start(){
  int port = 4545;

  if(getenv("DWARF_KEEPER_PORT")){
      port = atoi(getenv("DWARF_KEEPER_PORT"));
  }

  printf("[.] starting server at port %d...\n", port);

  mhd = MHD_start_daemon(0,
                       port,
                       NULL, NULL,      // accept policy  callback + argument
                       &process_request, NULL, // access handler callback + argument
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

      if(!mhd) return;

#ifdef __linux__
      static pthread_mutex_t foo_mutex = PTHREAD_MUTEX_INITIALIZER;
      pthread_mutex_lock(&foo_mutex);
#endif

      FD_ZERO(&sr); FD_ZERO(&sw); FD_ZERO(&sx);

      if( MHD_YES != MHD_get_fdset(mhd,&sr,&sw,&sx,&max_fd)){
          perror("[!] MHD_fet_fdset failed: ");
          exit(1);
      }

      tv.tv_sec = 0;
      tv.tv_usec = 10;

      select (max_fd + 1, &sr, &sw, &sx, &tv);
      MHD_run(mhd);

#ifdef __linux__
      pthread_mutex_unlock(&foo_mutex);
#endif
}
