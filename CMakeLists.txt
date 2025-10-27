cmake_minimum_required(VERSION 3.0...3.12)

if(NOT DEFINED ENV{PS2DEV})
	message(FATAL_ERROR "Please kindly set $PS2DEV environment variable")
endif()
set(PS2DEV $ENV{PS2DEV})

if(NOT DEFINED ENV{GSKIT})
	message(FATAL_ERROR "Please kindly set $GSKIT environment variable")
endif()
set(GSKIT $ENV{GSKIT})

include(${PS2DEV}/ps2sdk/ps2dev.cmake)

list(APPEND CMAKE_FIND_ROOT_PATH ${GSKIT})

project(DS2Tester)

set(CMAKE_EXECUTABLE_SUFFIX .elf)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

find_library(LIBPNG_LIBRARY NAMES png)
find_path(LIBPNG_INCLUDE_DIR NAMES png.h)

if(NOT LIBPNG_LIBRARY)
	message(FATAL_ERROR "libpng not found")
else ()
	add_library(png STATIC IMPORTED)
	set_target_properties(png PROPERTIES IMPORTED_LOCATION "${LIBPNG_LIBRARY}")
	add_definitions(-DHAVE_LIBPNG)
endif()

find_library(GSKIT_LIBRARY NAMES gskit PATHS "/usr/local/ps2dev/gsKit/lib")
add_library(gskit STATIC IMPORTED)
set_target_properties(gskit PROPERTIES IMPORTED_LOCATION "${GSKIT_LIBRARY}")

find_library(DMAKIT_LIBRARY NAMES dmakit PATHS "/usr/local/ps2dev/gsKit/lib")
add_library(dmakit STATIC IMPORTED)
set_target_properties(dmakit PROPERTIES IMPORTED_LOCATION "${DMAKIT_LIBRARY}")

find_library(GSTOOLKIT_LIBRARY NAMES gskit_toolkit PATHS "/usr/local/ps2dev/gsKit/lib")
add_library(gskit_toolkit STATIC IMPORTED)
set_target_properties(gskit_toolkit PROPERTIES IMPORTED_LOCATION "${GSTOOLKIT_LIBRARY}")

include_directories(include)

set(CMAKE_SOURCE_DIR src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

add_executable(ds2test
		src/main.c
		src/pad.c
		src/sprite.c
)

target_include_directories(ds2test PRIVATE "/usr/local/ps2dev/gsKit/include")

target_link_libraries(ds2test PRIVATE gskit_toolkit gskit dmakit)

target_link_options(ds2test PRIVATE "-lpng" "-lpad" "-lz" "-lgskit" "-lgskit_toolkit" "-ldmakit" )

file(COPY ./assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})