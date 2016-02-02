add_executable (maintest main.cpp)

if(NOT CMAKE_BUILD_TYPE MATCHES "Coverage")
  add_test (main_test maintest)
endif()
