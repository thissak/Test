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
    glEnable(GL_MULTISAMPLE);
    m_box = Mesh::CreateBox();
    m_plane = Mesh::CreatePlane();

    //skybox program
    m_skyboxProgram = Program::Create("./shader/skybox.vs", "./shader/skybox.fs");
    if(!m_skyboxProgram){
        spdlog::error("fail to load skyboxProgram");
        return false;
    }
    //envMap program
    m_envMapProgram = Program::Create("./shader/env_map.vs", "./shader/env_map.fs");
    int a = 1;
    if (!m_envMapProgram){
        spdlog::error("fail to load envMapProgram");
        return false;
    }
    //post program
    m_postProgram = Program::Create("./shader/texture.vs", "./shader/gamma.fs");
    if(!m_postProgram){
        spdlog::error("fail to load postProgram");
        return false;
    }

    m_textureProgram = Program::Create("./shader/texture.vs", "./shader/texture.fs");
    if(!m_textureProgram){
        spdlog::error("failed to load textureProgram");
        return false;
    }
    m_simpleProgram  = Program::Create("./shader/simple.vs",  "./shader/simple.fs");
    if (!m_simpleProgram){
        spdlog::error("failed to load simpleProgram");
        return false;
        }
    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if (!m_program){
        spdlog::error("failed to load Program");
        return false;
    }
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    //skybox texture
    auto cubeRight = Image::Load("./img/skybox/right.jpg", false);
    auto cubeLeft = Image::Load("./img/skybox/left.jpg", false);
    auto cubeTop = Image::Load("./img/skybox/top.jpg", false);
    auto cubeBottom = Image::Load("./img/skybox/bottom.jpg", false);
    auto cubeFront = Image::Load("./img/skybox/front.jpg", false);
    auto cubeBack = Image::Load("./img/skybox/back.jpg", false);
    m_cubeTexture = CubeTexture::CreateFromImages({
        cubeRight.get(),
        cubeLeft.get(),
        cubeTop.get(),
        cubeBottom.get(),
        cubeFront.get(),
        cubeBack.get(),
    });

    //grass
    m_grassTexture = Texture::CreateFromImage(Image::Load("./img/grass.png").get());
    m_grassProgram = Program::Create("./shader/grass.vs", "./shader/grass.fs");
    m_grassPos.resize(50000);

    for (size_t i= 0; i < m_grassPos.size(); i++){
        m_grassPos[i].x = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 5.0f; //-5~5
        m_grassPos[i].z = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 5.0f;
        m_grassPos[i].y = glm::radians((float)rand()) / (float)RAND_MAX * 360.0f;
    }
    m_grassInstance = VertexLayout::Create();
    m_grassInstance->Bind();
    m_plane->GetVertexBuffer()->Bind();
    m_grassInstance->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    m_grassInstance->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal));
    m_grassInstance->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texcoord)); 

    m_grassPosBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, m_grassPos.data(), 
        sizeof(glm::vec3), m_grassPos.size()); 
    m_grassPosBuffer->Bind();
    m_grassInstance->SetAttrib(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glVertexAttribDivisor(3, 1);
    m_plane->GetIndexBuffer()->Bind();
    
    TexturePtr darkGrayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4,
            glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)).get());

    TexturePtr grayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4,
            glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());

    m_windowTexture = Texture::CreateFromImage(
        Image::Load("./img/blending_transparent_window.png").get());

    m_planeMat = Material::Create();
    m_planeMat->diffuse = Texture::CreateFromImage(
        Image::Load("./img/marble.jpg").get());
        m_planeMat->specular = grayTexture;
        m_planeMat->shininess = 128.0f;

    m_box1Mat = Material::Create();
    m_box1Mat->diffuse = Texture::CreateFromImage(
        Image::Load("./img/container.jpg").get());
    m_box1Mat->specular = darkGrayTexture;
    m_box1Mat->shininess = 16.0f;

    m_box2Mat = Material::Create();
    m_box2Mat->diffuse = Texture::CreateFromImage(
        Image::Load("./img/container2.png").get());
    m_box2Mat->specular = Texture::CreateFromImage(
        Image::Load("./img/container2_specular.png").get());
    m_box2Mat->shininess = 64.0f;
    
    return true;
}


void Context::Render() {
    std::default_random_engine generator;  // 랜덤 넘버 생성기
    std::uniform_real_distribution<float> distribution(0.0f, 3.0f);  // 초당 20도에서 100도 사이의 랜덤 회전 속도
    if (ImGui::Begin("ui window")){
        // light parameters
        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.5, 0.0f, 3000.0f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.5f, 0.0f, 90.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
            ImGui::Checkbox("flash light", &m_flashLightMode);
            ImGui::Checkbox("l.blinn", &m_blinn);
        }
            

        ImGui::Checkbox("animation", &m_animation); 

        if(ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))){
           glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w); 
        }
        ImGui::DragFloat("gamma", &m_gamma, 0.01f, 0.0f, 2.0f);
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
    // ... imgui codes end

    // m_framebuffer->Bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_cameraFront = 
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) * 
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) * 
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 100.0f);
    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
    
    //skybox
    auto skyboxModelTransform = 
        glm::translate(glm::mat4(1.0), m_cameraPos) *
        glm::scale(glm::mat4(1.0), glm::vec3(50.0f));
    m_skyboxProgram->Use();
    m_cubeTexture->Bind();
    m_skyboxProgram->SetUniform("skybox", 0);
    m_skyboxProgram->SetUniform("transform", projection * view * skyboxModelTransform);
    m_box->Draw(m_skyboxProgram.get());

    // Light mesh render setting
    auto lightModelTransform = glm::translate(glm::mat4(1.0), m_light.position) *
                               glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
    m_simpleProgram->Use();
    m_simpleProgram->SetUniform("color", glm::vec4((m_light.diffuse + m_light.ambient), 1.0f));
    m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
    m_box->Draw(m_simpleProgram.get());

    glm::vec3 lightPos = m_light.position;
    glm::vec3 lightDir = m_light.direction;
    if (m_flashLightMode){
        lightPos = m_cameraPos;
        lightDir = m_cameraFront;
    }

    m_program->Use();
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", lightPos);
    m_program->SetUniform("light.direction", lightDir);
    m_program->SetUniform("light.cutoff", glm::vec2(
        cosf(glm::radians(m_light.cutoff[0])),
        cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))
    ));
    m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);

    //blinn
    m_program->SetUniform("blinn", (m_blinn ? 1 : 0));

    //plane
    auto modelTransform = 
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(10.f, 1.0f, 10.0f));
    auto transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_planeMat->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    //envMap cube
    modelTransform = 
        glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.75f, -2.0f)) * 
        glm::rotate(glm::mat4(1.0f), glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    m_envMapProgram->Use();
    m_envMapProgram->SetUniform("model", modelTransform);
    m_envMapProgram->SetUniform("view", view);
    m_envMapProgram->SetUniform("projection", projection);
    m_envMapProgram->SetUniform("cameraPos", m_cameraPos);
    m_cubeTexture->Bind();
    m_envMapProgram->SetUniform("skybox", 0);
    m_box->Draw(m_envMapProgram.get());

    //box1
    modelTransform = 
        glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f,0.0f)) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f)); 
    transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_box1Mat->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    //box2
    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_box2Mat->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());


    //window
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_textureProgram->Use();
    m_windowTexture->Bind();
    m_textureProgram->SetUniform("tex", 0);

    modelTransform = 
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 4.0f));
    transform = projection * view * modelTransform;
    m_textureProgram->SetUniform("transform", transform);
    m_plane->Draw(m_textureProgram.get());

    modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.5f, 5.0f));
    transform = projection * view * modelTransform;
    m_textureProgram->SetUniform("transform", transform);
    m_plane->Draw(m_textureProgram.get());

    modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.5f, 6.0f));
    transform = projection * view * modelTransform;
    m_textureProgram->SetUniform("transform", transform);
    m_plane->Draw(m_textureProgram.get());

    //grass
    glEnable(GL_BLEND);
    m_grassProgram->Use();
    m_grassProgram->SetUniform("tex", 0);
    m_grassTexture->Bind();
    m_grassInstance->Bind();
    modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
    transform = projection * view * modelTransform;
    m_grassProgram->SetUniform("transform", transform);
    glDrawElementsInstanced(GL_TRIANGLES,
        m_plane->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0,
    m_grassPosBuffer->GetCount());
    
    // Framebuffer::BindToDefault();

    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // m_postProgram->Use();
    // m_postProgram->SetUniform("transform", 
    //     glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f)));
    // m_postProgram->SetUniform("gamma", m_gamma);
    // m_framebuffer->GetColorAttachment()->Bind();
    // m_postProgram->SetUniform("tex", 0);
    // m_plane->Draw(m_postProgram.get());
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

    m_framebuffer = Framebuffer::Create(Texture::Create(width, height, GL_RGBA));
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
