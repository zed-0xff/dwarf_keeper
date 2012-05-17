CC     := g++
CFLAGS := -g -I/usr/include -lSDL -lcurl
#OBJS   := main.o fetcher.o drawer.o
OBJS   := main.cpp fetcher.cpp drawer.cpp screen.cpp

all: dv

dv: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $<

#%.o: %.cpp
#	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm dv *.o
