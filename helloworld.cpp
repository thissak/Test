#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

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

    // glfw 윈도우 생성, 실패하면 에러 출력 후 종료
    spdlog::info("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, 
        nullptr, nullptr);
    if (!window){
        spdlog::error("failed to create glfw window");
        glfwTerminate();
        return -1;
    }

    // glfw 루프실행, 윈도우 close 버튼을 누르면 정상 종료
    spdlog::info("Start main loop");
    while (!glfwWindowShouldClose(window)){
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}