if (${CMAKE_MAJOR_VERSION} GREATER 2)
    cmake_policy (SET CMP0037 OLD)
endif ()
add_executable (test main arbitrary complexity property timed_test)
