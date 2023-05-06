cmake_minimum_required (VERSION 3.0)
project (testinator LANGUAGES CXX)

# Includes for this project
include_directories ("${PROJECT_SOURCE_DIR}/src/include")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default C++ standard: C++14
if(NOT CXX_STD)
  set(CXX_STD 14)
endif()

# Set up tests
enable_testing()
include(CTest)

# Compile flags
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(MY_CXX_FLAGS_LIST
    /wd4101
    )
  string(REPLACE ";" " " MY_CXX_FLAGS "${MY_CXX_FLAGS_LIST}")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MY_CXX_FLAGS}")
else()
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

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++${CXX_STD} ${MY_CXX_FLAGS}")

  # Debug/Release
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -march=native -mtune=native -DNDEBUG")
  set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
endif()

# Clang/GCC specifics
if("x${CMAKE_CXX_COMPILER_ID}" MATCHES "x.*Clang")
  if(NOT GCOV)
    find_program(GCOV llvm-cov)
  endif()
  set(lcov_args "--gcov-tool" "${CMAKE_SOURCE_DIR}/llvm-gcov.sh")
  if(ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined,integer -fno-omit-frame-pointer -fno-sanitize=unsigned-integer-overflow")
  endif()
elseif("x${CMAKE_CXX_COMPILER_ID}" MATCHES "xGNU")
  if(NOT GCOV)
    find_program(GCOV gcov)
  endif()
  set(lcov_args "--gcov-tool" "${GCOV}")
endif()

# Coverage information
if(CMAKE_BUILD_TYPE MATCHES "Coverage")
  find_program(LCOV lcov)
  find_program(GENHTML genhtml)
  set(lcov_args ${lcov_args} "-q" "--compat-libtool")
endif()

macro(SHIFT result listvar)
  list(GET ${listvar} 0 ${result})
  list(REMOVE_AT ${listvar} 0)
endmacro(SHIFT)

macro(ADD_COVERAGE executable suffix)
  if(CMAKE_BUILD_TYPE MATCHES "Coverage")
    if (GCOV AND LCOV AND GENHTML)
      set(args ${ARGV})
      list(REMOVE_AT args 0)
      list(REMOVE_AT args 0)
      string(REPLACE ";" " " arglist "${args}")
      add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/lcov/${executable}.${suffix}.total
        DEPENDS ${executable}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running ${LCOV} for ${executable} ${arglist}"
        COMMAND mkdir -p lcov
        COMMAND ${LCOV} ${lcov_args} -z -d .
        COMMAND ${LCOV} ${lcov_args} --no-external -c -b "${CMAKE_SOURCE_DIR}" -d . -o lcov/${executable}.${suffix}.before -i
        COMMAND ${executable} ${args}
        COMMAND ${LCOV} ${lcov_args} --no-external -c -b "${CMAKE_SOURCE_DIR}" -d . -o lcov/${executable}.${suffix}.after
        COMMAND ${LCOV} ${lcov_args} -a lcov/${executable}.${suffix}.before -a lcov/${executable}.${suffix}.after -o lcov/${executable}.${suffix}.total
        )
      message(STATUS
        "Coverage added for ${executable} ${arglist}:\n"
        "  gcov: ${GCOV}\n"
        "  lcov: ${LCOV}\n"
        "  genhtml: ${GENHTML}\n")
    else()
      message(WARNING
        "Coverage not available for ${executable} ${arglist}:\n"
        "  gcov: ${GCOV}\n"
        "  lcov: ${LCOV}\n"
        "  genhtml: ${GENHTML}\n")
    endif()
  endif()
endmacro()

macro(EXCLUDE_COVERAGE executable suffix excludes)
  if(CMAKE_BUILD_TYPE MATCHES "Coverage")
    if (GCOV AND LCOV AND GENHTML)
      add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/lcov/${executable}.${suffix}.final
        DEPENDS lcov/${executable}.${suffix}.total
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Excluding ${excludes} for ${executable}"
        COMMAND ${LCOV} ${lcov_args} -r lcov/${executable}.${suffix}.total "${excludes}" -o lcov/${executable}.${suffix}.final
        )
    endif()
  endif()
endmacro()

# Pipe separate tests into ctest
# Adapted from https://github.com/ChaiScript/ChaiScript/blob/develop/CMakeLists.txt
macro(ADD_INDIVIDUAL_TESTS executable type)
  set(test_path $ENV{PATH})
  get_target_property(target_files ${executable} SOURCES)
  foreach(source ${target_files})
    file(READ "${source}" contents)
    string(REGEX MATCHALL "DEF_${type}\\([ ]*[^, ]+[ ]*,[ ]*[^, ]+[ ]*[,\\)]" found_tests ${contents})
    foreach(hit ${found_tests})
      string(REGEX REPLACE "DEF_${type}\\([ ]*([^, ]+)[ ]*,[ ]*[^, ]+[ ]*[,\\)]" "\\1" tname ${hit})
      string(REGEX REPLACE "DEF_${type}\\([ ]*[^, ]+[ ]*,[ ]*([^, ]+)[ ]*[,\\)]" "\\1" sname ${hit})
      set(test_name ${executable}.${sname}.${tname})
      add_test(NAME ${test_name}
        COMMAND "${executable}" --testName=${tname} --suiteName=${sname})
      set_tests_properties(${test_name} PROPERTIES TIMEOUT 30 ENVIRONMENT "PATH=${test_path}")
    endforeach()
  endforeach()
endmacro()

macro(ADD_TESTINATOR_TESTS executable)
  ADD_INDIVIDUAL_TESTS(${executable} "TEST")
  ADD_INDIVIDUAL_TESTS(${executable} "PROPERTY")
  ADD_INDIVIDUAL_TESTS(${executable} "TIMED_TEST")
  ADD_INDIVIDUAL_TESTS(${executable} "COMPLEXITY_PROPERTY")
endmacro()

add_subdirectory (src/test)
add_subdirectory (src/maintest)

if(CMAKE_BUILD_TYPE MATCHES "Coverage")
  # run test_testinator 3 times for all, suite and individual test
  ADD_COVERAGE(test_${PROJECT_NAME} 0)
  EXCLUDE_COVERAGE(test_${PROJECT_NAME} 0 "*.cpp")
  ADD_COVERAGE(test_${PROJECT_NAME} 1 "--testName=Foo")
  EXCLUDE_COVERAGE(test_${PROJECT_NAME} 1 "*.cpp")
  ADD_COVERAGE(test_${PROJECT_NAME} 2 "--suiteName=Foo")
  EXCLUDE_COVERAGE(test_${PROJECT_NAME} 2 "*.cpp")

  # run maintest 4 times, to exercise code paths in main.h
  ADD_COVERAGE(maintest 0 "--output=" "--testName=" "--suiteName=" "--numChecks=1" "--seed=1" "--alpha" "--verbose" "--nocolor")
  EXCLUDE_COVERAGE(maintest 0 "*.cpp")
  ADD_COVERAGE(maintest 1 "--help")
  EXCLUDE_COVERAGE(maintest 1 "*.cpp")
  ADD_COVERAGE(maintest 2 "--testName=Foo")
  EXCLUDE_COVERAGE(maintest 2 "*.cpp")
  ADD_COVERAGE(maintest 3 "--suiteName=Foo")
  EXCLUDE_COVERAGE(maintest 3 "*.cpp")

  add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/lcov/index.html
    DEPENDS lcov/test_testinator.0.final
    DEPENDS lcov/test_testinator.1.final
    DEPENDS lcov/test_testinator.2.final
    DEPENDS lcov/maintest.0.final
    DEPENDS lcov/maintest.1.final
    DEPENDS lcov/maintest.2.final
    DEPENDS lcov/maintest.3.final
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running ${GENHTML}"
    COMMAND ${LCOV} ${lcov_args} -a lcov/test_testinator.0.final -a lcov/test_testinator.1.final -a lcov/test_testinator.2.final -a lcov/maintest.0.final -a lcov/maintest.1.final -a lcov/maintest.2.final -a lcov/maintest.3.final -a lcov/maintest.1.final -o lcov/final
    COMMAND ${LCOV} ${lcov_args} --list lcov/final
    COMMAND ${GENHTML} -q -o lcov --legend --sort -p "${CMAKE_BINARY_DIR}" lcov/final
    )
  add_custom_target(coverage
    DEPENDS ${CMAKE_BINARY_DIR}/lcov/index.html
    COMMENT "LCOV report at lcov/index.html"
    )
endif()
