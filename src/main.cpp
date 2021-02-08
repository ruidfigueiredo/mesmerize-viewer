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

#include "Picture.h"
#include "ResolutionScaleCalculator.h"

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

static void loadImageIntoSlot0(const std::string &path)
{
    int width, height, bytesPerPixel;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *imageData = stbi_load(path.c_str(), &width, &height, &bytesPerPixel, 0);
    ASSERT(imageData != nullptr);

    //iir_gauss_blur(width, height, bytesPerPixel, imageData, 20.0f);
    unsigned char *output_pixels = (unsigned char *)malloc(1980 * 1080 * bytesPerPixel);
    stbir_resize_uint8(imageData, width, height, 0, output_pixels, 1980, 1080, 0, bytesPerPixel);

    unsigned int textureId;
    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1980, 1080, 0, GL_RGB, GL_UNSIGNED_BYTE, output_pixels);
    delete output_pixels;

    stbi_image_free((void *)imageData);
}

static void loadImageAndCreateVertexBuffer(const std::string &path)
{
    int width, height, bytesPerPixel;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *imageData = stbi_load(path.c_str(), &width, &height, &bytesPerPixel, 0);
    ASSERT(imageData != nullptr);

    int maxResolution;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxResolution);
    //maxResolution = 1980;
    unsigned char *output_pixels = nullptr;
    if (width >= maxResolution || height >= maxResolution)
    {
        std::cout << "Image is too large for video card memory (max is " << maxResolution << "x" << maxResolution << std::endl;
        int max = std::max(width, height);
        std::cout << "Image is " << width << "x" << height << ", using " << max << " to determine scale factor" << std::endl;
        float scaleFactor = 1.0f * maxResolution / max;
        std::cout << "Scale factor is: " << scaleFactor << std::endl;
        int finalWidth = (int)std::floor(width * scaleFactor);
        int finalHeight = (int)std::floor(height * scaleFactor);
        std::cout << "Image is now " << finalWidth << "x" << finalHeight << std::endl;
        output_pixels = (unsigned char *)malloc(width * height * bytesPerPixel);
        stbir_resize_uint8(imageData, width, height, 0, output_pixels, finalWidth, finalHeight, 0, bytesPerPixel);
    }
    stbi_image_free(imageData);
    delete output_pixels;
}

int main(void)
{
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

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 300 es");

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

    VertexArray va;

    //loadImageAndCreateVertexBuffer("./example.jpg");
    //loadImageIntoSlot0("./example.jpg");
    Picture pl1{std::make_shared<ResolutionScaleCalculator>()};
    auto pl1LoadResult = pl1.Load("/home/rdfi/Pictures/bird.JPG", 0);
    Picture pl2{std::make_shared<ResolutionScaleCalculator>()};
    pl2.Load("/home/rdfi/Pictures/IMG_20201103_133032-EFFECTS.jpg", 1);

    // float triangleVertices[] = {
    //     0.0f, 0.0f, 0.0f, 0.0f,
    //     640.0f, 0.0f, 1.0f, 0.0f,
    //     640.0f, 480.0f, 1.0f, 1.0f,
    //     0.0f, 480.0f, 0.0f, 1.0f};
    //float *triangleVertices = pl1LoadResult.VertexCoordinates[0];
    std::cout << "----------------------\n";
    for (auto i = 0; i < pl1LoadResult.VertexCoordinates.size(); i++)
    {
        if (i > 0 && i % 4 == 0)
            std::cout << std::endl;
        std::cout << std::to_string(pl1LoadResult.VertexCoordinates[i]) + " ";
    }
    std::cout << "\n----------------------\n";
    float *triangleVertices = &pl1LoadResult.VertexCoordinates[0];
    VertexBuffer vb(triangleVertices, sizeof(float) * pl1LoadResult.VertexCoordinates.size());
    VertexBufferLayout twoFloatBufferLayout;
    twoFloatBufferLayout.Push<float>(2);
    twoFloatBufferLayout.Push<float>(2);
    va.AddBuffer(vb, twoFloatBufferLayout);

    uint indexes[] = {
        0, 1, 2,
        2, 3, 0};
    IndexBuffer ib(indexes, 6);
    va.AddBuffer(ib);
    va.Bind();

    glfwSwapInterval(1);

    Renderer renderer;
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

            if (counter > 5)
            {
                selectedTextureSlot = (selectedTextureSlot == 0 ? 1 : 0);
                pl1.Load("/home/rdfi/Pictures/IMG_20201103_133032-EFFECTS.jpg", 0);
                counter = 0;
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
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
        GL_CALL(renderer.Draw(va, program));

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