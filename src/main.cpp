#include "context.h"
#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

void OnMouseLeftClick(GLFWwindow* window, int button, int action, int modifier){
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);
    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    context->MouseButton(button, action, x, y);
}

void OnCursorPos(GLFWwindow* window, double x, double y){
    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    context->MouseMove(x, y);
}

void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    glViewport(0, 0, width, height);
    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    context->Reshape(width, height);
}

void OnKeyEvent(GLFWwindow* window,
    int key, int scancode, int action, int mods) {
    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
        key, scancode,
        action == GLFW_PRESS ? "Pressed" :
        action == GLFW_RELEASE ? "Released" :
        action == GLFW_REPEAT ? "Repeat" : "Unknown",
        mods & GLFW_MOD_CONTROL ? "C" : "-",
        mods & GLFW_MOD_SHIFT ? "S" : "-",
        mods & GLFW_MOD_ALT ? "A" : "-");
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(int arc, const char** argv)
{
    spdlog::info("Start program");

    // glfw 라이브러리 초기화, 실패하면 에러 출력 후 종료
    spdlog::info("initialize glfw");
    if (!glfwInit()){
        const char* description = nullptr;
        glfwGetError(&description);
        spdlog::error("failed to inintialize glfw: {}", description);
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); 

    // glfw 윈도우 생성, 실패하면 에러 출력 후 종료
    spdlog::info("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, 
        nullptr, nullptr);
    if (!window){
        spdlog::error("failed to create glfw window");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // glad를 활용한 OpenGL 함수 로딩
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }
    auto glVersion = glGetString(GL_VERSION);
    std::string versionString(reinterpret_cast<const char*>(glVersion));
    SPDLOG_INFO("OpenGL context version: {}", versionString);

    auto imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();

    auto context = Context::Create();
    if(!context){
        spdlog::error("failed to create context");
        glfwTerminate();
        return -1;
    }
    
    glfwSetWindowUserPointer(window, context.get());

    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetKeyCallback(window, OnKeyEvent);
    glfwSetCursorPosCallback(window, OnCursorPos);
    glfwSetMouseButtonCallback(window, OnMouseLeftClick);

    // glfw 루프실행, 윈도우 close 버튼을 누르면 정상 종료
    spdlog::info("Start main loop");
    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        context->ProcessInput(window);
        context->Render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    context.reset();

    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);

    glfwTerminate();
    return 0;
}