cmake_minimum_required(VERSION 3.19.2)
project(HelloWorld)
add_executable(${PROJECT_NAME} src/main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_NAME run)
