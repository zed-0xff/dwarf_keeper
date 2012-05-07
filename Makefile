CC     := g++
CFLAGS := -g -m32 -Iinclude -Ilibmicrohttpd/src/include


LIBRARY := injectlib-memserver.dylib
OBJS    := 8-injectlib-http.cpp unicode.cpp dwarf.cpp item.cpp html.cpp clothes_controller.cpp common.h items_controller.cpp http_request.cpp units_controller.cpp mem_class.cpp soul.cpp unit.cpp skill.cpp reference.cpp controller.cpp building.cpp screen.cpp trade_controller.cpp item_type.cpp buildings_controller.cpp screen_controller.cpp window.cpp live_controller.cpp offscreen_renderer.cpp binary_template.cpp

UNAME = $(shell uname)
ifeq ($(UNAME), Darwin)
    CFLAGS := $(CFLAGS) -flat-namespace -dynamiclib
    OBJS   := $(OBJS) _osx.h
endif

ifeq ($(UNAME), Linux)
    CFLAGS := $(CFLAGS) -shared -fPIC -lpthread -ldl
    OBJS   := $(OBJS) _linux.h
endif

LIBMICROHTTPD_A := libmicrohttpd/src/daemon/.libs/libmicrohttpd.a


all: $(LIBRARY)

clean:
	rm *.o target *.dylib a.out

$(LIBRARY): $(OBJS) $(LIBMICROHTTPD_A)
	$(CC) $(CFLAGS) $< $(LIBMICROHTTPD_A) -o $@

item_type.cpp: item_type.rb
	./item_type.rb > item_type.cpp

$(LIBMICROHTTPD_A): libmicrohttpd/Makefile
	$(MAKE) -C libmicrohttpd

libmicrohttpd/Makefile: libmicrohttpd/configure
	cd libmicrohttpd && \
	export CFLAGS="-m32 -fPIC" && \
	./configure --disable-https --disable-dauth --disable-curl --disable-largefile --disable-postprocessor

libmicrohttpd:
	svn co https://gnunet.org/svn/libmicrohttpd
