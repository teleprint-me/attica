# @file tests/core/CMakeLists.txt
set(UNIT_TESTS
    "test_logger"
    "test_memory"
    "test_node"
)

set(INPUT_DIR ${PROJECT_SOURCE_DIR}/tests/core)
set(OUTPUT_DIR ${PROJECT_SOURCE_DIR}/build/tests/core)

foreach(unit_test IN LISTS UNIT_TESTS)
    add_executable(${unit_test} ${INPUT_DIR}/${unit_test}.c)
    target_link_libraries(${unit_test} dsa)
    target_include_directories(${unit_test} PRIVATE ${PROJECT_SOURCE_DIR}/include)
    set_target_properties(${unit_test} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
endforeach()
