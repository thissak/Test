#include "context.h"
#include "image.h"
#include <imgui.h>
#include <random>


ContextUPtr Context::Create(){
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}
    

bool Context::Init(){
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
    };

    uint32_t indices[] = {
    0,  2,  1,  2,  0,  3,
    4,  5,  6,  6,  7,  4,
    8,  9, 10, 10, 11,  8,
    12, 14, 13, 14, 12, 15,
    16, 17, 18, 18, 19, 16,
    20, 22, 21, 22, 20, 23,
    };

    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 8 * 6 * 4);
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 3);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, sizeof(float) * 6);

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 36);

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/lighting.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/lighting.fs", GL_FRAGMENT_SHADER);
    if(!vertShader || !fragShader)
        return false;
    spdlog::info("vertex shader id: {}", vertShader->Get());
    spdlog::info("fragment shader id: {}", fragShader->Get());

    // create program
    m_program = Program::Create({fragShader, vertShader});
    if(!m_program)
        return false;
    spdlog::info("program id: {}", m_program->Get());

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    // load image
    auto image = Image::Load("./img/container.jpg");
    m_texture0 = Texture::CreateFromImage(image.get());

    auto image2 = Image::Load("./img/awesomeface.png");
    m_texture1 = Texture::CreateFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture0->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture1->Get());

    // m_program->Use();
    // m_program->SetUniform("tex", 0);
    // m_program->SetUniform("tex2", 1);

    return true;
}


void Context::Render() {
    std::default_random_engine generator;  // 랜덤 넘버 생성기
    std::uniform_real_distribution<float> distribution(0.0f, 3.0f);  // 초당 20도에서 100도 사이의 랜덤 회전 속도
    if (ImGui::Begin("ui window")){
        // light parameters
        if (ImGui::CollapsingHeader("light"), ImGuiTreeNodeFlags_DefaultOpen) {
        ImGui::DragFloat3("light pos", glm::value_ptr(m_lightPos), 0.01f);
        ImGui::ColorEdit3("light color", glm::value_ptr(m_lightColor));
        ImGui::ColorEdit3("object color", glm::value_ptr(m_objectColor));
        ImGui::SliderFloat("ambient strength", &m_ambientStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("specular strength", &m_specularStrength, 0.0f, 1.0f);
        ImGui::DragFloat("specular shininess", &m_specularShininess, 1.0f, 1.0f, 256.0f);

    }

        ImGui::Checkbox("animation", &m_animation); 

        if(ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))){
           glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w); 
        }
        ImGui::Separator();
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.01f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.01f, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")){
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
            m_cameraPitch = 0.0f;
            m_cameraYaw = 0.0f;
        }
    }
    ImGui::End();

    std::vector<glm::vec3> cubePos = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_cameraFront = 
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) * 
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) * 
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto projection = glm::perspective(glm::radians(45.0f), 
        (float)m_width / (float)m_height, 0.01f, 20.0f);
    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    // Light mesh render setting
    auto lightModelTransform = glm::translate(glm::mat4(1.0), m_lightPos) *
        glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
    m_program->Use();
    m_program->SetUniform("lightPos", m_lightPos);
    m_program->SetUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    m_program->SetUniform("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
    m_program->SetUniform("ambientStrength", 1.0f);
    m_program->SetUniform("transform", projection * view * lightModelTransform);
    m_program->SetUniform("modelTransform", lightModelTransform);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("lightPos", m_lightPos);
    m_program->SetUniform("lightColor", m_lightColor);
    m_program->SetUniform("objectColor", m_objectColor);
    m_program->SetUniform("ambientStrength", m_ambientStrength);
    m_program->SetUniform("specularStrength", m_specularStrength);
    m_program->SetUniform("specularShininess", m_specularShininess);


    for (size_t i = 0; i < cubePos.size(); i++){
        float randomSpeed = distribution(generator);
        auto& pos = cubePos[i];
        auto model = glm::translate(glm::mat4(1.0f), pos);
        float angle = glm::radians((float)glfwGetTime() * 30.0f * randomSpeed + 20.0f * (float)i);
        model = glm::rotate(model, (m_animation ? angle : 0.0f), glm::vec3(1.0f, 0.5f, 0.5f));
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
        m_program->SetUniform("modelTransform", model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    } 

}

void Context::ProcessInput(GLFWwindow *window){
    if (!m_cameraControl)
        return;
    const float cameraSpeed = 0.05f;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;


}

void Context::Reshape(int width, int height){
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y){
    if (!m_cameraControl)
        return;
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.3f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f) m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f) m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y){
    if (button == GLFW_MOUSE_BUTTON_RIGHT){
        if (action == GLFW_PRESS){
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE){
            m_cameraControl = false;
        }
    }
}
