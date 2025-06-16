# @file tests/CMakeLists.txt

# Define test modules
set(TEST_MODULES
    "core"
    "sort"
    "allocator"
    "container"
    "utf8"
)

# Set input and output directories
set(INPUT_DIR ${PROJECT_SOURCE_DIR}/tests)
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/tests)

# Log test directory and output directory
message(STATUS "Test Sources Directory: ${INPUT_DIR}")
message(STATUS "Test Executables Directory: ${OUTPUT_DIR}")
message(STATUS "Test Names: ${TEST_MODULES}")

# Create test executables
foreach (module IN LISTS TEST_MODULES)
    add_subdirectory(${module})
endforeach()
