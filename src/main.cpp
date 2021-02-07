#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

#include <string>
#include <signal.h>
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

#include "PictureLoader.h"
#include "ResolutionScaleCalculator.h"
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

#define ASSERT(x) \
    if (!(x))     \
    raise(SIGTRAP)

#define GL_CALL(x)   \
    ClearGlErrors(); \
    x;               \
    ASSERT(GlLogCall(#x, __FILE__, __LINE__))

static void
ClearGlErrors()
{
    while (glGetError())
        ;
}

static void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *message,
                const void *userParam)
{
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}

static bool GlLogCall(const char *functionCall, const char *file, int lineNo)
{
    GLenum error = glGetError();
    if (error == GL_NO_ERROR)
        return true;

    std::cout << "[OpenGL Error] ("
              << std::hex
              << error
              << std::dec << ") "
              << functionCall << " "
              << file << ": " << lineNo << std::endl;
    return false;
}

std::vector<std::string> getPicturePaths() {
    std::vector<std::string> result;
    std::string path = "/home/rdfi/Pictures/Croatia";
    for(const auto & entry: std::filesystem::directory_iterator(path)){
        result.push_back(entry.path());
    }
    return result;
}

int main(void)
{
    auto picturePaths = getPicturePaths();
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */

        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    int maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    std::cout << "MAX Texture size is " << maxTextureSize << std::endl;

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    ShaderProgram program;
    program.AddVertexShader("shaders/vertex.shader");
    program.AddFragmentShader("shaders/fragment.shader");
    program.Bind();

    glfwSwapInterval(1);

    Renderer renderer;
    //loadImageAndCreateVertexBuffer("./example.jpg");
    //loadImageIntoSlot0("./example.jpg");
    PictureLoader pl1{std::make_shared<ResolutionScaleCalculator>(), 
        std::make_shared<ImagePositionCalculator>(std::make_shared<ResolutionScaleCalculator>())};
    pl1.Load(picturePaths[0]);

    /* Loop until the user closes the window */
    float factor = 1.0f;
    float delta = 0.0001f;
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
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {

            static int counter = 0;
            static int previous = 0;

            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text."); // Display some text (you can use a format strings too)
            ImGui::SliderFloat3("TranslateA", &translateVectorA.x, 0.0f, 1080.0f);
            ImGui::SliderFloat3("TranslateB", &translateVectorB.x, 0.0f, 1080.0f);
            ImGui::SliderFloat("Scale", &factor, 0.0f, 5.0f);
            ImGui::SliderFloat("Blend", &blendValue, 0.0f, 1.0f);    // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
            if (previous != counter) {
                pl1.Load(picturePaths[counter % picturePaths.size()]);
                previous = counter;
            }

        }

        program.SetUniformi("oddTextureSlot", 0);
        program.SetUniformi("evenTextureSlot", 1);
        program.SetUniformf("blendValue", blendValue);
        //glm::mat4 projection = glm::ortho(0.0f * factor + (factor - 1.0f) * delta * 30.0f, 320.0f * factor + (factor - 1.0f) * delta * 30.0f, 0 * factor, 240 * factor, -1.0f, 1.0f);
        glm::mat4 projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f);

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
        glm::mat4 model{1.0f};
        program.SetUniformMat4f("mvp", projection * view * model);

        //GL_CALL(glDrawElements(GL_TRIANGLES, ib.GetNumberOfElements(), GL_UNSIGNED_INT, nullptr));
        GL_CALL(pl1.Render());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        if (factor < 0.9f || factor > 1.2f)
        {
            delta = -delta;
        }
        factor -= delta;

        /* Poll for and process events */
        glfwPollEvents();
    }

    //glDeleteProgram(programId);
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}