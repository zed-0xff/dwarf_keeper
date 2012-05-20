CC     := g++
CFLAGS := -g -m32 -Iinclude -Ilibmicrohttpd/src/include -Ilibdisasm-0.23/libdisasm -Ilibsnappy


LIBRARY := injectlib-memserver.dylib
OBJS    := 8-injectlib-http.cpp unicode.cpp dwarf.cpp item.cpp html.cpp clothes_controller.cpp common.h items_controller.cpp http_request.cpp units_controller.cpp mem_class.cpp soul.cpp unit.cpp skill.cpp reference.cpp controller.cpp building.cpp screen.cpp trade_controller.cpp item_type.cpp buildings_controller.cpp screen_controller.cpp window.cpp live_controller.cpp offscreen_renderer.cpp binary_template.cpp x86_emu.cpp copied_screen.cpp

UNAME = $(shell uname)
ifeq ($(UNAME), Darwin)
    CFLAGS := $(CFLAGS) -flat-namespace -dynamiclib
    OBJS   := $(OBJS) _osx.h
endif

ifeq ($(UNAME), Linux)
    CFLAGS := $(CFLAGS) -shared -fPIC -lpthread -ldl -std=c++0x
    OBJS   := $(OBJS) _linux.h
endif

LIBMICROHTTPD_A := libmicrohttpd/src/daemon/.libs/libmicrohttpd.a
LIBDISASM_A     := libdisasm-0.23/libdisasm/.libs/libdisasm.a
LIBSNAPPY_A     := libsnappy/.libs/libsnappy.a

STATIC_LIBS     := $(LIBMICROHTTPD_A) $(LIBDISASM_A) $(LIBSNAPPY_A)

all: $(LIBRARY)

clean:
	rm *.o *.dylib a.out

$(LIBRARY): $(OBJS) $(STATIC_LIBS) Makefile
	@echo -e '\E[47;35m'"\033[1m=================>\033[0m"
	$(CC) $(CFLAGS) $< $(STATIC_LIBS) -o $@

item_type.cpp: item_type.rb
	./item_type.rb > item_type.cpp


###############################################################################


$(LIBMICROHTTPD_A): libmicrohttpd/Makefile
	$(MAKE) -C libmicrohttpd

libmicrohttpd/Makefile: libmicrohttpd/configure
	cd libmicrohttpd && \
	export CFLAGS="-g -m32 -fPIC" && \
	./configure --disable-https --disable-dauth --disable-curl --disable-largefile --disable-postprocessor

libmicrohttpd/configure:
	svn co https://gnunet.org/svn/libmicrohttpd


###############################################################################


$(LIBDISASM_A): libdisasm-0.23/Makefile
	$(MAKE) -C libdisasm-0.23

libdisasm-0.23/Makefile: libdisasm-0.23/configure
	cd libdisasm-0.23 && \
	export CFLAGS="-m32 -fPIC" && \
	./configure

libdisasm-0.23/configure:
	tar xzf libdisasm-0.23.tar.gz


###############################################################################


$(LIBSNAPPY_A): libsnappy/Makefile
	$(MAKE) -C libsnappy

libsnappy/Makefile: libsnappy/configure
	cd libsnappy && \
	export CXXFLAGS="-g -m32 -fPIC" && \
	./configure

libsnappy/configure: libsnappy/autogen.sh
	cd libsnappy && sh autogen.sh

libsnappy/autogen.sh:
	svn co http://snappy.googlecode.com/svn/trunk/ libsnappy
