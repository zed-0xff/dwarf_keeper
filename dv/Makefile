CC     := g++
CFLAGS := -g -I/usr/include -lcurl `sdl-config --cflags --libs`
#OBJS   := main.o fetcher.o drawer.o
OBJS   := main.cpp fetcher.cpp drawer.cpp screen.cpp event_sender.cpp local_screen.cpp screen_fetcher.cpp

CFLAGS := $(CFLAGS) -I sdl_picofont-1.2/include

UNAME = $(shell uname)
ifeq ($(UNAME), Linux)
    CFLAGS := $(CFLAGS) -std=c++0x
endif
ifeq ($(UNAME), Darwin)
    CFLAGS := $(CFLAGS) -D__osx__
endif

PICOFONT_SRCS := sdl_picofont-1.2/src/font.c sdl_picofont-1.2/src/spf.c

all: dv

dv: $(OBJS) $(PICOFONT_SRCS)
	$(CC) $(CFLAGS) -o $@ $< $(PICOFONT_SRCS)

#%.o: %.cpp
#	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm dv *.o
