cmake_minimum_required(VERSION 3.13.4)
project(MesmerizeClient)
add_subdirectory(vendor)
set(ProjectFiles 
    "src/IndexBuffer.cpp"
    "src/Renderer.cpp"
    "src/VertexArray.cpp"
    "src/VertexBufferLayout.cpp"
    "src/ShaderProgram.cpp"
    "src/VertexBuffer.cpp"
    "src/ImagePositionCalculator.cpp"
    "src/ResolutionScaleCalculator.cpp"
    "src/Picture.cpp"
    "src/DeviceInformation.cpp"
    "src/TimingFunctions/TimingFunction.cpp"
    "src/TimingFunctions/EaseInOut.cpp"
    "src/main.cpp"
    )
add_executable(${PROJECT_NAME} ${ProjectFiles})

set(IncludeDirectories 
    "vendor/glfw/include"
    "vendor/glew-2.1.0/include"
    )
target_include_directories(${PROJECT_NAME} PRIVATE ${IncludeDirectories})
target_link_libraries(${PROJECT_NAME} glfw glew glm imgui iir_gauss stb)
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_NAME run)

file(COPY example.jpg DESTINATION ${CMAKE_BINARY_DIR})

add_custom_command(TARGET MesmerizeClient PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/shaders)

add_subdirectory(src/tests)