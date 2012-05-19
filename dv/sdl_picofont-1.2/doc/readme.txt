sdl_picofont - readme

About:
	sdl_picofont is a very, very (very) simple and small font library for SDL. It doesn't feature any fancy stuff like UTF8 support (although that might change). In fact it only features one built in 8x8 1bit bitmap font. What it DOES support however is newlines and tabs, unlike some other, more extensive font libraries for SDL (yes SDL_ttf, I'm looking at you). 

Why:
	There are a lot of text output libraries for SDL, a quick search on "font" under libraries on libsdl.org gives you like 10 hits. But most of them are a bit too complex for really small applications and most of them require you to have some external font files to load. This library is designed for people who just want som damned text output on the screen.

How:
	See doc/usage.txt

License:
	See doc/license.txt (GPLv2)

Authors:
	Fredrik Hultin

With additional patches from:
	Saul D "KrayZier" Beniquez
	

Contact:
	Homepage: 	http://nurd.se/~noname/
	E-mail: 	noname @ the very same domain

Requirements:
	sdl_picofont requires SDL to work.

Changelog:
	Version 1.2
		- Added cmake-file to build as a library
		- Added include guard for .h-file (which I apparently forgot, thanks Saul!)
		- Renamed spf.h to SDL_picofont.h to match SDL-naming conventions
		- Added a pkg-config file
		- Added extern "C"-stuff in files, since it's now a "real library" (Saul again)
		  (It's still possible to just include the files in your project)

	Version 1.1
		- Fixed warning when compiling with g++
		- Removed crap about buffer overflows in the docs
		- Renamed the render functions (hope it didn't break anyone's code)

	Version 1.0
		- Initial release
