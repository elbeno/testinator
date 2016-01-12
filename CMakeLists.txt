cmake_minimum_required (VERSION 2.8.12)
project (TESTINATOR)
include_directories ("${PROJECT_SOURCE_DIR}/src/include")

set(MY_CXX_FLAGS_LIST
	-pedantic
	-Wall
	-Wextra
	-Wcast-align
	-Wcast-qual
	-Wctor-dtor-privacy
	-Wdisabled-optimization
	-Wformat=2
	-Winit-self
	-Wmissing-include-dirs
	-Wold-style-cast
	-Woverloaded-virtual
	-Wredundant-decls
	-Wshadow
	-Wsign-conversion
	-Wsign-promo
	-Wstrict-overflow=5
	-Wswitch-default
	-Wundef
	-Werror
)
string(REPLACE ";" " " MY_CXX_FLAGS "${MY_CXX_FLAGS_LIST}")

option(USE_CLANG "build application with clang" ON)

if (FALSE AND USE_CLANG)
    SET (CMAKE_C_COMPILER             "/usr/bin/clang")
    SET (CMAKE_C_FLAGS                "-Wall -std=c99")
    SET (CMAKE_C_FLAGS_DEBUG          "-g")
    SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_CXX_COMPILER "/usr/bin/clang++")
    SET (CMAKE_CXX_FLAGS    "-std=c++1y -stdlib=libc++ ${MY_CXX_FLAGS}")
    SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_AR      "/usr/bin/llvm-ar")
    SET (CMAKE_LINKER  "/usr/bin/llvm-ld")
    SET (CMAKE_NM      "/usr/bin/llvm-nm")
    SET (CMAKE_OBJDUMP "/usr/bin/llvm-objdump")
    SET (CMAKE_RANLIB  "/usr/bin/llvm-ranlib")
else ()
    SET (CMAKE_CXX_FLAGS    "-std=c++1y ${MY_CXX_FLAGS}")
endif ()

add_subdirectory (src/test)
add_subdirectory (src/maintest)
