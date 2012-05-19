PROJECT(SDL_picofont C)

SET(TARGET_NAME "SDL_picofont")

SET(SOURCE_FILES
	src/spf.c
	src/font.c
)

SET(CPACK_PACKAGE_NAME SDL_picofont)
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_VENDOR "Fredrik Hultin <noname@nurd.se>")
SET(CPACK_PACKAGE_CONTACT "Fredrik Hultin <noname@nurd.se>")
SET(CPACK_PACKAGE_DESCRIPTION "SDL_picofont is a minimalistic font library for SDL")

INCLUDE_DIRECTORIES(
        include
)

ADD_LIBRARY(SDL_picofont STATIC ${SOURCE_FILES})

# Check if cmake has the deb-file generator
IF(EXISTS "${CMAKE_ROOT}/Modules/CPackDeb.cmake")
	SET(CPACK_GENERATOR DEB)

	FIND_PROGRAM(DPKG "dpkg")
	IF(NOT DPKG STREQUAL "DPKG-NOTFOUND")
		EXEC_PROGRAM(dpkg ARGS --print-architecture OUTPUT_VARIABLE CPACK_SYSTEM_NAME)
	ELSE(NOT DPKG STREQUAL "DPKG-NOTFOUND")
		SET(CPACK_SYSTEM_NAME "i386")
	ENDIF(NOT DPKG STREQUAL "DPKG-NOTFOUND")
	INCLUDE(CPack)
ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPackDeb.cmake")

# SDL

FIND_PATH(SDL_INC SDL/SDL.h)
IF(SDL_INC EQUAL "SDL_INC-NOTFOUND")
        MESSAGE(FATAL_ERROR "Unable to find include path for SDL")
ELSE(SDL_INC EQUAL "SDL_INC-NOTFOUND")
        INCLUDE_DIRECTORIES(${SDL_INC}/SDL)
ENDIF(SDL_INC EQUAL "SDL_INC-NOTFOUND")

Find_Package ( SDL REQUIRED )

# Workaround for the non-working REQUIRED flag 
# (according to some tutorial at: http://gpwiki.org/index.php/C:How_to_set_up_your_SDL_Build_Environment#CMake )

if (NOT SDL_FOUND)
        message ( FATAL_ERROR "Unable to find SDL library" )
else (NOT SDL_FOUND)
        TARGET_LINK_LIBRARIES (${TARGET_NAME} ${SDL_LIBRARY} SDLmain)
endif (NOT SDL_FOUND)

INSTALL(TARGETS SDL_picofont ARCHIVE DESTINATION lib)
INSTALL(FILES include/SDL_picofont.h DESTINATION include/SDL)
INSTALL(FILES pkg-config/SDL_picofont.pc DESTINATION lib/pkgconfig)
