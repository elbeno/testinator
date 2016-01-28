cmake_minimum_required (VERSION 2.8)
project (testinator)

# Includes for this project
include_directories ("${PROJECT_SOURCE_DIR}/src/include")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default C++ standard: C++14
if(CXX_STD)
else()
  set(CXX_STD 14)
endif()

# Set up tests
enable_testing()
include(CTest)

set(MY_CXX_FLAGS_LIST
  -ftemplate-backtrace-limit=0
  -ffunction-sections
  -Wall -Wextra -Werror -pedantic-errors
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
  )
string(REPLACE ";" " " MY_CXX_FLAGS "${MY_CXX_FLAGS_LIST}")

if(CI_BUILD)
else()
  set(CMAKE_CXX_FLAGS "-fsanitize=address,undefined,integer -fno-omit-frame-pointer -fno-sanitize=unsigned-integer-overflow")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${CXX_STD} ${MY_CXX_FLAGS}")

# Debug/Release
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -march=native -mtune=native -DNDEBUG")

# Clang/GCC specifics
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
elseif(CMAKE_COMPILER_IS_GNUCXX)
endif()

add_subdirectory (src/test)
add_subdirectory (src/maintest)
