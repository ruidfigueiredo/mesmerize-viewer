#define ENABLE_GL_DEBUG_OUTPUT 0

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

//#define ENABLE_IMGUI 1

#ifdef ENABLE_IMGUI
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#endif
#include "Renderer.h"
#include "DeviceInformation.h"
#include "Picture.h"
#include "CheckGlErrors.h"
#include "PictureRendererWithTransition.h"
#include "ManualTicker.h"
#include "GetSuffledPicturePaths.h"
#include "MesmerizeOptions.h"




int main(int argc, const char** argv)
{
    if (argc == 1){
        MesmerizeOptions::PrintOptions();
        exit(0);
    }

    MesmerizeOptions options;
    try {
        options = MesmerizeOptions::FromArgs(argc, argv);
    }catch(std::runtime_error e) {
        std::cout << "Unknown option: " << e.what() << "\n\n";
        MesmerizeOptions::PrintOptions();
        exit(1);
    }

    auto picturePaths = GetShuffledPicturePaths::GetShuffledPicturePaths(options.PathToPicturesFolder);
    if (picturePaths.empty()) {
        std::cout << "Folder " << options.PathToPicturesFolder << " has no pictures (.jpg or .png)\n";
        return 0;
    }

    PictureRendererWithTransition pictureRendererWithTransition(
            options.PanAnimationDuration,
            options.ZoomAnimationDuration,
            options.OpacityAnimationDuration,
            options.PanPercentage,
            options.ZoomPercentage);

    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    const GLFWvidmode *vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int deviceWidth = vidMode->width;
    int deviceHeight = vidMode->height;

    if (options.IsFullScreen) {
        if (options.IsResolutionSetToMax) {
            window = glfwCreateWindow(deviceWidth, deviceHeight, "Mesmerize", glfwGetPrimaryMonitor(), NULL);
        } else {
            window = glfwCreateWindow(options.ResolutionX, options.ResolutionY, "Mesmerize", glfwGetPrimaryMonitor(),NULL);
        }
    }else {
        if (options.IsResolutionSetToMax) {
            window = glfwCreateWindow(deviceWidth, deviceHeight, "Mesmerize", NULL, NULL);
        } else {
            window = glfwCreateWindow(options.ResolutionX, options.ResolutionY, "Mesmerize", NULL, NULL);
        }
    }

    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE){
#ifdef ENABLE_IMGUI
            GL_CALL(ImGui_ImplGlfw_Shutdown());
            GL_CALL(ImGui_ImplOpenGL3_Shutdown());
            GL_CALL(ImGui::DestroyContext());
#endif
            glfwTerminate();
            exit(0);
        }
    });

    DeviceInformation::init(window, deviceWidth, deviceHeight);

#ifdef ENABLE_IMGUI
    GL_CALL(ImGui::CreateContext());
    GL_CALL(ImGui_ImplGlfw_InitForOpenGL(window, true));
    GL_CALL(ImGui_ImplOpenGL3_Init("#version 300 es"));
#endif

    GLenum err = glewInit();

    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

#if ENABLE_GL_DEBUG_OUTPUT
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    // During init, enable debug output
    GL_CALL(glEnable(GL_DEBUG_OUTPUT));
    GL_CALL(glDebugMessageCallback(MessageCallback, 0));
#endif

    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL_CALL(glBlendEquation(GL_FUNC_ADD));


    Picture::InitShaders();

    glfwSwapInterval(1);

    Renderer renderer;
    /* Loop until the user closes the window */
    static int counter = 0;
    ManualTicker manualTicker([&] {
        int index = ++counter % picturePaths.size();
        manualTicker.Pause(15000); //some pictures take a considerable amount of time to load in the pi, more than 15 secs probably there was an error and the event is not coming
        pictureRendererWithTransition.Load(picturePaths[index], [&manualTicker]() {
            manualTicker.Resume();
        });
    }, options.DisplayPictureDuration);

    while (!glfwWindowShouldClose(window))
    {
        manualTicker.Tick(); //will trigger the change of picture if enough time has passed
        /* Render here */
        renderer.Clear();

#ifdef ENABLE_IMGUI
        GL_CALL(ImGui_ImplOpenGL3_NewFrame());
        GL_CALL(ImGui_ImplGlfw_NewFrame());
        GL_CALL(ImGui::NewFrame());
        {
            GL_CALL(ImGui::Begin("Debug"));

            if (ImGui::Button("<"))
            { // Buttons return true when clicked (most widgets return true when edited/activated)
                int index = --counter % picturePaths.size();
                pictureRendererWithTransition.Load(picturePaths[index]);
            }

            ImGui::SameLine();
            if (ImGui::Button(">"))
            { // Buttons return true when clicked (most widgets return true when edited/activated)
                int index = ++counter % picturePaths.size();
                pictureRendererWithTransition.Load(picturePaths[index]);
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
#endif

        pictureRendererWithTransition.Render();

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
