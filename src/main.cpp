#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

#include <string>
#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <iir_gauss_blur.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "ShaderProgram.h"
#include "Renderer.h"

#include "Picture.h"
#include "ResolutionScaleCalculator.h"
#include "CheckGlErrors.h"

#include <vector>
#include <sys/types.h>
#include <dirent.h>

std::vector<std::string> GetFilePathsInFolder(std::string pathToFolder)
{
    std::vector<std::string> results;
    DIR *dirp = opendir(pathToFolder.c_str());
    struct dirent *dp;
    while((dp = readdir(dirp)) != nullptr) {
        if (dp->d_type == DT_REG)
            results.push_back(pathToFolder + "/" + dp->d_name);
    }
    closedir(dirp);
    return results;
}


int main(void)
{
    auto results = GetFilePathsInFolder("/home/rdfi/Pictures/Croatia");
    for(auto filePath : results) {
        std::cout << filePath << std::endl;
    }
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GL_CALL(ImGui::CreateContext());
    GL_CALL(ImGui_ImplGlfw_InitForOpenGL(window, true));
    GL_CALL(ImGui_ImplOpenGL3_Init("#version 300 es"));

    GLenum err = glewInit();

    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */

        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    int maxTextureSize;
    GL_CALL(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize));
    std::cout << "MAX Texture size is " << maxTextureSize << std::endl;

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

    auto rsc = std::make_shared<ResolutionScaleCalculator>();
    Picture nudibranch{rsc, std::make_shared<ImagePositionCalculator>(rsc)};
    nudibranch.Load("/home/rdfi/Pictures/Croatia/nudibranche.jpg", 0);
    Picture peixeMau{rsc, std::make_shared<ImagePositionCalculator>(rsc)};
    peixeMau.Load("/home/rdfi/Pictures/Croatia/peixe mau.jpg", 1);

    glfwSwapInterval(1);

    Renderer renderer;
    /* Loop until the user closes the window */
    float factor = 1.0f;
    float delta = 0.0005f;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    glm::vec3 translateVectorA(0.0f, 0.0f, 0.0f);
    glm::vec3 translateVectorB(0.0f, 0.0f, 0.0f);
    int selectedTextureSlot = 0;
    float blendValue = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();
        //glClear(GL_COLOR_BUFFER_BIT);

        // Start the Dear ImGui frame
        GL_CALL(ImGui_ImplOpenGL3_NewFrame());
        GL_CALL(ImGui_ImplGlfw_NewFrame());
        GL_CALL(ImGui::NewFrame());
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static int counter = 0;

            GL_CALL(ImGui::Begin("Hello, world!")); // Create a window called "Hello, world!" and append into it.

            GL_CALL(ImGui::Text("This is some useful text.")); // Display some text (you can use a format strings too)
            GL_CALL(ImGui::SliderFloat3("TranslateA", &translateVectorA.x, 0.0f, 1080.0f));
            GL_CALL(ImGui::SliderFloat3("TranslateB", &translateVectorB.x, 0.0f, 1080.0f));
            GL_CALL(ImGui::SliderFloat("Scale", &factor, 0.0f, 5.0f));
            GL_CALL(ImGui::SliderFloat("Blend", &blendValue, 0.0f, 1.0f));    // Edit 1 float using a slider from 0.0f to 1.0f
            GL_CALL(ImGui::ColorEdit3("clear color", (float *)&clear_color)); // Edit 3 floats representing a color

            if (ImGui::Button("Button")){ // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
                nudibranch.Load(results[counter], 0);
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        program.SetUniformf("blendValue", blendValue);
        //glm::mat4 projection = glm::ortho(0.0f * factor + (factor - 1.0f) * delta * 30.0f, 320.0f * factor + (factor - 1.0f) * delta * 30.0f, 0 * factor, 240 * factor, -1.0f, 1.0f);
        glm::mat4 projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f);

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(100*factor, 100*factor, 0));
        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1.1f, 1.1f, 1.0f));
        program.SetUniformMat4f("mvp", projection * view * model);

        //GL_CALL(glDrawElements(GL_TRIANGLES, ib.GetNumberOfElements(), GL_UNSIGNED_INT, nullptr));
        program.Bind();
        program.SetUniformi("textureSlot", 1);
        program.SetUniformf("blendValue", 1.0f);
        peixeMau.Render();
        program.SetUniformi("textureSlot", 0);
        program.SetUniformf("blendValue", blendValue);
        nudibranch.Render();
        //peixeMau.Render();
        //GL_CALL(renderer.Draw(va, program));

        GL_CALL(ImGui::Render());
        GL_CALL(ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()));
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        if (factor < 0.1f || factor > 1.0f)
        {
            delta = -delta;
        }
        factor -= delta;

        /* Poll for and process events */
        glfwPollEvents();
    }

    //glDeleteProgram(programId);
    GL_CALL(ImGui_ImplGlfw_Shutdown());
    GL_CALL(ImGui_ImplOpenGL3_Shutdown());
    GL_CALL(ImGui::DestroyContext());
    glfwTerminate();
    return 0;
}