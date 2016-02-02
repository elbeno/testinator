add_executable (test_${PROJECT_NAME}
  main.cpp arbitrary.cpp complexity.cpp
  property.cpp timed_test.cpp)

if(NOT CMAKE_BUILD_TYPE MATCHES "Coverage")
  ADD_TESTINATOR_TESTS (test_${PROJECT_NAME})
else()
  ADD_COVERAGE (test_${PROJECT_NAME})
endif()
