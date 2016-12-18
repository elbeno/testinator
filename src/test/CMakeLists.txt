add_executable (test_${PROJECT_NAME}
  main.cpp arbitrary.cpp capture.cpp complexity.cpp
  property.cpp timed_test.cpp)
ADD_TESTINATOR_TESTS (test_${PROJECT_NAME})

