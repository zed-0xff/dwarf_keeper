CC     := g++
CFLAGS := -g -I/usr/include -lcurl `sdl-config --cflags --libs`
#OBJS   := main.o fetcher.o drawer.o
OBJS   := main.cpp fetcher.cpp drawer.cpp remote_screen.cpp event_sender.cpp local_screen.cpp screen_fetcher.cpp

CFLAGS := $(CFLAGS) -Isdl_picofont-1.2/include -Ilibsnappy

UNAME = $(shell uname)
ifeq ($(UNAME), Linux)
    CFLAGS := $(CFLAGS) -std=c++0x
endif
ifeq ($(UNAME), Darwin)
    CFLAGS := $(CFLAGS) -D__osx__
endif

PICOFONT_SRCS := sdl_picofont-1.2/src/font.c sdl_picofont-1.2/src/spf.c

LIBSNAPPY_A     := libsnappy/.libs/libsnappy.a
STATIC_LIBS     := $(LIBSNAPPY_A)

all: dv

dv: $(OBJS) $(PICOFONT_SRCS) $(STATIC_LIBS)
	$(CC) $(CFLAGS) -o $@ $< $(PICOFONT_SRCS) $(STATIC_LIBS)

#%.o: %.cpp
#	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm dv *.o


###############################################################################


$(LIBSNAPPY_A): libsnappy/Makefile
	$(MAKE) -C libsnappy

libsnappy/Makefile: libsnappy/configure
	cd libsnappy && ./configure

libsnappy/configure: libsnappy/autogen.sh
	cd libsnappy && sh autogen.sh

libsnappy/autogen.sh:
	svn co http://snappy.googlecode.com/svn/trunk/ libsnappy
