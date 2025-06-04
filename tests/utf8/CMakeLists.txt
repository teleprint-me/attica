# @file tests/utf8/CMakeLists.txt

# Define test units
set(TEST_UNITS
    "test_utf8_byte"
    "test_utf8_raw"
)

set(INPUT_DIR ${PROJECT_SOURCE_DIR}/tests/utf8)
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/tests/utf8)

foreach(test IN LISTS TEST_UNITS)
    add_executable(${test} ${INPUT_DIR}/${test}.c)
    target_link_libraries(${test} dsa)
    target_include_directories(${test} PRIVATE ${PROJECT_SOURCE_DIR}/include)
    set_target_properties(${test} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
    add_custom_target("run_${test}" COMMAND ${test} DEPENDS ${test} COMMENT "Running tests for ${test}")
    add_test(NAME ${test} COMMAND ${test} WORKING_DIRECTORY ${OUTPUT_DIR})
endforeach()
