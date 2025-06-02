# Root CMakeLists.txt

cmake_minimum_required(VERSION 3.14)
project(dsa VERSION 0.1.0 DESCRIPTION "ANSI C library for Data Structures and Algorithms")

# Set C standard rules
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Enable Shared Objects
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Add sanitizers for memory safety
# Ref: https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html
# Ref: https://developers.redhat.com/blog/2018/03/21/compiler-and-linker-flags-gcc

# Common warning flags
set(COMMON_WARNING_FLAGS "-Wall -Wextra -Wpedantic -Werror -Wformat-security -Wshadow -fexceptions")

# Additional Debug-only flags (sanitizers and memory safety checks)
set(DEBUG_SANITIZERS "-fsanitize=address,undefined -fno-omit-frame-pointer")
set(DEBUG_EXTRA_WARNINGS "-Wformat -Wnull-dereference -Wdouble-promotion")

# Static analysis flags for catching common security issues
set(DEBUG_ANALYSIS "-Wanalyzer-double-free -Wanalyzer-file-leak -Wanalyzer-malloc-leak -Wanalyzer-null-dereference -Wanalyzer-out-of-bounds -Wanalyzer-va-list-leak")

# Enable debugging level 3 for macros
set(DEBUG_FLAGS "-g3")

# Apply flags conditionally based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_WARNING_FLAGS} ${DEBUG_SANITIZERS} ${DEBUG_EXTRA_WARNINGS} ${DEBUG_ANALYSIS} ${DEBUG_FLAGS}")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_WARNING_FLAGS}")
endif()

# Set the output directory for built binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add library
add_library(dsa SHARED
    "src/core/logger.c"
    "src/core/memory.c"
    "src/core/node.c"

    "src/test/unit.c"

    "src/map/open-address.c"

    "src/allocator/arena.c"
    "src/allocator/pool.c"
    "src/allocator/stack.c"
    "src/allocator/lease.c"

    # "src/numeric/constant.c"
    # "src/numeric/lehmer.c"
    # "src/numeric/type.c"
    # "src/numeric/activation.c"

    # "src/utf8/byte.c"
    # "src/utf8/raw.c"
)
target_include_directories(dsa PUBLIC include)
target_link_libraries(dsa PUBLIC m rt pthread pcre2-8)

enable_testing()
add_subdirectory(tests)

# Custom doc target for generating docs
add_custom_target(doc
    COMMAND doxygen ${CMAKE_SOURCE_DIR}/doxy.conf
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen"
)

add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running all tests"
)

# Custom clean target for removing generated files
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR}/lib
    COMMENT "Cleaning all generated files"
)
