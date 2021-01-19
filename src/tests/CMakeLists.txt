cmake_minimum_required(VERSION 3.19)

set(TEST_FILES 
    ResolutionScaleCalculatorTests.cpp
    ImagePositionCalculatorTests.cpp
)
set(FILES_UNDER_TEST
    "../ResolutionScaleCalculator.cpp"
    "../ImagePositionCalculator.cpp"
)

add_executable(test test_main.cpp ${TEST_FILES} ${FILES_UNDER_TEST})
target_link_libraries(test gtest)
target_include_directories(test PRIVATE ${CMAKE_SOURCE_DIR}/vendor/googletest/googletest/include)
set_target_properties(test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})