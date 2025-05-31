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

# Setup submodule, module, and test names
set(SUBMODULES logger)
set(MODULES node linked_list) # Add more modules here as needed

# Set the output directory for built binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Enable testing
enable_testing()

# Add dependencies
foreach(SUBMODULE IN LISTS SUBMODULES)
    add_subdirectory(mods/${SUBMODULE})
endforeach()

# Collect all source files into lists
foreach(MODULE IN LISTS MODULES)
    list(APPEND LIB_HEADERS "include/${MODULE}.h")
    list(APPEND LIB_SOURCES "src/${MODULE}.c")
endforeach()

# Add the library using the collected sources
add_library(dsa ${LIB_SOURCES})

set_target_properties(
    dsa PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "${LIB_HEADERS}"
)

target_include_directories(dsa PUBLIC include)
target_link_libraries(dsa PUBLIC logger)

# Add and link test executables
foreach(MODULE IN LISTS MODULES)
    add_executable("test_${MODULE}" "tests/test_${MODULE}.c")
    target_link_libraries("test_${MODULE}" PRIVATE dsa m)
    set_target_properties("test_${MODULE}" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
    add_test(NAME "test_${MODULE}" COMMAND "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_${MODULE}")
endforeach()

# Install rules for the library
install(
    TARGETS dsa
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)
