all: injectlib-memserver.dylib

injectlib-memserver.dylib: 8-injectlib-http.cpp unicode.cpp dwarf.cpp item.cpp html.cpp clothes_controller.cpp common.h items_controller.cpp http_request.cpp units_controller.cpp mem_class.cpp soul.cpp unit.cpp skill.cpp reference.cpp controller.cpp building.cpp screen.cpp trade_controller.cpp item_type.cpp buildings_controller.cpp screen_controller.cpp window.cpp live_controller.cpp offscreen_renderer.cpp
	g++ -Iinclude -g -arch i386 -flat_namespace -dynamiclib libmicrohttpd.a -o $@ $<

item_type.cpp: item_type.rb
	./item_type.rb > item_type.cpp

clean:
	rm *.o target *.dylib a.out
