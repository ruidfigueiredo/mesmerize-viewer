cmake_minimum_required(VERSION 3.19)

add_subdirectory(glfw)
add_subdirectory(glew-2.1.0/build/cmake)
add_subdirectory(glm)
add_subdirectory(googletest)
set(IMGUI_FILES 
    "imgui/imgui/imgui.cpp"
    "imgui/imgui/imgui_tables.cpp"
    "imgui/imgui/imgui_draw.cpp"
    "imgui/imgui/imgui_widgets.cpp"
    "imgui/imgui/backends/imgui_impl_glfw.cpp"
    "imgui/imgui/backends/imgui_impl_opengl3.cpp"
)

add_library(imgui ${IMGUI_FILES})
target_include_directories(imgui PUBLIC   
    "glew-2.1.0/include" 
    "glfw/include"
    "imgui"
    "imgui/imgui"
)


add_library(iir_gauss "iir_gauss_blur.cpp")
target_include_directories(iir_gauss PUBLIC "shfcl")

add_library(stb "stb/stb_image.cpp" "stb/stb_image_resize.cpp")
target_include_directories(stb PUBLIC "stb")

