#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

#include <string>
#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <iir_gauss_blur.h>
#include <glm/glm.hpp>
#ifdef ENABLE_IMGUI
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#endif
#include <glm/gtc/matrix_transform.hpp>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "ShaderProgram.h"
#include "Renderer.h"
#include "DeviceInformation.h"

#include "Picture.h"
#include "ResolutionScaleCalculator.h"
#include "CheckGlErrors.h"
#include "TimingFunctions/TimingFunction.h"
#include "TimingFunctions/EaseInOut.h"

#include <vector>
#include <sys/types.h>
#include <dirent.h>

bool endsWith(std::string str, std::string ending)
{
    if (str.length() < ending.length())
        return false;
    auto res = str.substr(str.length() - ending.length());
    return (res == ending);
}

std::vector<std::string> GetFilePathsInFolder(std::string pathToFolder)
{
    std::vector<std::string> results;
    DIR *dirp = opendir(pathToFolder.c_str());
    struct dirent *dp;
    while ((dp = readdir(dirp)) != nullptr)
    {
        if (dp->d_type == DT_REG && endsWith(dp->d_name, "JPG"))
            results.push_back(pathToFolder + "/" + dp->d_name);
    }
    closedir(dirp);
    return results;
}

int main(void)
{
    EaseInOut easeInOutTimingFunction {15000};
    TimingFunction& timingFunctoin = easeInOutTimingFunction;
    
    auto results = GetFilePathsInFolder("/home/rdfi/Pictures/Croatia");
    //auto results = GetFilePathsInFolder("/media/rdfi/EvoNtfs/ToSaveAndMaybeNotToSave (copy)");
    for (auto filePath : results)
    {
        std::cout << filePath << std::endl;
    }
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    const GLFWvidmode *vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int deviceWidth = vidMode->width;
    int deviceHeight = vidMode->height;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(0.8 * deviceWidth, 0.8 * deviceHeight, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

#ifdef ENABLE_IMGUI
    GL_CALL(ImGui::CreateContext());
    GL_CALL(ImGui_ImplGlfw_InitForOpenGL(window, true));
    GL_CALL(ImGui_ImplOpenGL3_Init("#version 300 es"));
#endif 
    GLenum err = glewInit();

    DeviceInformation::init(window, 0.8 * deviceWidth, 0.8 * deviceHeight);
    // std::cout << "-------------------------------------\n";
    // std::cout << "DeviceInformation::getDeviceWidth()" << DeviceInformation::getDeviceWidth() << "\n";
    // std::cout << "DeviceInformation::getDeviceHeight()" << DeviceInformation::getDeviceHeight() << "\n";
    // std::cout << "DeviceInformation::getMaxTextureSize()" << DeviceInformation::getMaxTextureSize() << "\n";
    // std::cout << "-------------------------------------\n";
    // return 0;

    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */

        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    // During init, enable debug output
    GL_CALL(glEnable(GL_DEBUG_OUTPUT));
    GL_CALL(glDebugMessageCallback(MessageCallback, 0));

    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL_CALL(glBlendEquation(GL_FUNC_ADD));

    ShaderProgram program;
    program.AddVertexShader("shaders/vertex.shader");
    program.AddFragmentShader("shaders/fragment.shader");
    program.Bind();

    auto resolutionScaleCalculator = std::make_shared<ResolutionScaleCalculator>();
    auto imagePositionCalculator = std::make_shared<ImagePositionCalculator>();
    Picture picture1{resolutionScaleCalculator, imagePositionCalculator};
    picture1.Load("/home/rdfi/Pictures/Croatia/peixe mau.jpg", 0);
    Picture picture2{resolutionScaleCalculator, imagePositionCalculator};
    picture2.Load("/home/rdfi/Pictures/IMG_20201103_133626.jpg", 1);

    glfwSwapInterval(1);

    Renderer renderer;
    /* Loop until the user closes the window */
    int selectedTextureSlot = 0;
    float blendValue = 1.0f;
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();
        //glClear(GL_COLOR_BUFFER_BIT);

#ifdef ENABLE_IMGUI
        // Start the Dear ImGui frame
        GL_CALL(ImGui_ImplOpenGL3_NewFrame());
        GL_CALL(ImGui_ImplGlfw_NewFrame());
        GL_CALL(ImGui::NewFrame());
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static int counter = 0;

            GL_CALL(ImGui::Begin("Debug"));                                // Create a window called "Hello, world!" and append into it.
            GL_CALL(ImGui::SliderFloat("Blend", &blendValue, 0.0f, 1.0f)); // Edit 1 float using a slider from 0.0f to 1.0f

            if (ImGui::Button("<"))
            { // Buttons return true when clicked (most widgets return true when edited/activated)
                picture1.Load(results[--counter], 0);
            }

            ImGui::SameLine();
            if (ImGui::Button(">"))
            { // Buttons return true when clicked (most widgets return true when edited/activated)
                picture1.Load(results[++counter], 0);
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
#endif

        program.SetUniformf("blendValue", blendValue);
        glm::mat4 projection = glm::ortho(50.0f, 1.0f * DeviceInformation::getWidth()-50.0f, 0.0f, 1.0f * DeviceInformation::getHeight(), -1.0f, 1.0f);
        
        glm::mat4 model = glm::scale(glm::mat4{1.0f}, glm::vec3{1.0f+0.01*timingFunctoin.GetValue(), 1.0f+0.01*timingFunctoin.GetValue(), 1.0f});
        glm::mat4 view = glm::translate(model, glm::vec3{50*timingFunctoin.GetValue(), 0.0f, 0.0f});
        program.SetUniformMat4f("mvp", projection * view * model);

        program.Bind();
        // program.SetUniformi("textureSlot", 1);
        // program.SetUniformf("blendValue", 1.0f);
        // picture2.Render();
        program.SetUniformi("textureSlot", 0);
        program.SetUniformf("blendValue", blendValue);
        picture1.Render();
#ifdef ENABLE_IMGUI
        GL_CALL(ImGui::Render());
        GL_CALL(ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()));
#endif
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

#ifdef ENABLE_IMGUI    
    GL_CALL(ImGui_ImplGlfw_Shutdown());
    GL_CALL(ImGui_ImplOpenGL3_Shutdown());
    GL_CALL(ImGui::DestroyContext());
#endif
    glfwTerminate();
    return 0;
}