all: injectlib-simple.dylib injectlib-tcp-server.dylib target injectlib-memserver1.dylib injectlib-memserver2.dylib injectlib-memserver.dylib

injectlib-simple.dylib: 4-injectlib-simple.c
	gcc -flat_namespace -dynamiclib -o $@ $<

injectlib-tcp-server.dylib: 5-injectlib-tcp-server.c
	gcc -flat_namespace -dynamiclib -o $@ $<

injectlib-memserver1.dylib: 6-injectlib-memserver.c
	gcc -arch i386 -flat_namespace -dynamiclib -o $@ $<

injectlib-memserver2.dylib: 7-injectlib-memserver.cpp
	g++ -arch i386 -flat_namespace -dynamiclib -o $@ $<

injectlib-memserver.dylib: 8-injectlib-http.cpp unicode.cpp dwarf.cpp item.cpp html.cpp clothes_controller.cpp common.h items_controller.cpp http_request.cpp units_controller.cpp mem_class.cpp soul.cpp unit.cpp skill.cpp reference.cpp
	g++ -g -arch i386 -flat_namespace -dynamiclib libmicrohttpd.a -o $@ $<


target: 3-target.c
	gcc -o target 3-target.c

clean:
	rm *.o target *.dylib a.out


run-simple: target injectlib-simple.dylib
	DYLD_INSERT_LIBRARIES="injectlib-simple.dylib" DYLD_FORCE_FLAT_NAMESPACE="1" ./target

run-tcp-server: target injectlib-tcp-server.dylib
	DYLD_INSERT_LIBRARIES="./injectlib-tcp-server.dylib" DYLD_FORCE_FLAT_NAMESPACE="1" ./target
