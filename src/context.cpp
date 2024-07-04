#include "context.h"
#include "image.h"
#include <imgui.h>
#include <random>


ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::DrawScene(const glm::mat4& view, 
    const glm::mat4& projection, const Program* program)
    {
        program->Use();
        auto modelTransform = 
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) * 
            glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 1.0f, 40.0f));
        auto transform = projection * view * modelTransform;
        program->SetUniform("transform", transform);
        program->SetUniform("modelTransform", modelTransform);
        m_planeMat->SetToProgram(program);
        m_box->Draw(program);

        // ssao test
        modelTransform = 
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.55f, 0.0f)) * 
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * 
        glm::scale(glm::mat4(1.0f),  glm::vec3(0.5f, 0.5f, 0.5f));
        transform = projection * view * modelTransform;
        program->SetUniform("transform", transform);
        program->SetUniform("modelTransform", modelTransform);
        m_model->Draw(program);

        modelTransform = 
            glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) * 
            glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
            glm::scale(glm::mat4(1.0f),  glm::vec3(1.5f, 1.5f, 1.5f));
        transform = projection * view * modelTransform;
        program->SetUniform("transform", transform);
        program->SetUniform("modelTransform", modelTransform);
        m_box1Mat->SetToProgram(program);
        m_box->Draw(program);

        modelTransform = 
            glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.75f, -4.0f)) * 
            glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
            glm::scale(glm::mat4(1.0f),  glm::vec3(1.5f, 1.5f, 1.5f));
        transform = projection * view * modelTransform;
        program->SetUniform("transform", transform);
        program->SetUniform("modelTransform", modelTransform);
        m_box2Mat->SetToProgram(program);
        m_box->Draw(program);

        modelTransform = 
            glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, 2.0f)) * 
            glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
            glm::scale(glm::mat4(1.0f),  glm::vec3(1.5f, 1.5f, 1.5f));
        transform = projection * view * modelTransform;
        program->SetUniform("transform", transform);
        program->SetUniform("modelTransform", modelTransform);
        m_box2Mat->SetToProgram(program);
        m_box->Draw(program);

        modelTransform = 
            glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 1.75f, 2.0f)) * 
            glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
            glm::scale(glm::mat4(1.0f),  glm::vec3(1.5f, 1.5f, 1.5f));
        transform = projection * view * modelTransform;
        program->SetUniform("transform", transform);
        program->SetUniform("modelTransform", modelTransform);
        m_box2Mat->SetToProgram(program);
        m_box->Draw(program);
    }
    

bool Context::Init(){
    glEnable(GL_MULTISAMPLE);
    m_box = Mesh::CreateBox();
    m_plane = Mesh::CreatePlane();

    m_ssaoSamples.resize(64);
    for (size_t i = 0; i < m_ssaoSamples.size(); i++) {
        // uniformly randomized point in unit hemisphere
        glm::vec3 sample(
            RandomRange(-1.0f, 1.0f),
            RandomRange(-1.0f, 1.0f),
            RandomRange(0.0f, 1.0f));
        sample = glm::normalize(sample) * RandomRange();

        // scale for slightly shift to center
        float t = (float)i / (float)m_ssaoSamples.size();
        float t2 = t * t;
        float scale = (1.0f - t2) * 0.1f + t2 * 1.0f;

        m_ssaoSamples[i] = sample * scale;
    }

    //ssao
    m_ssaoProgram = Program::Create("./shader/ssao.vs", "./shader/ssao.fs");
    m_model = Model::Load("./model/backpack.obj");
    //deffered program
    m_deferGeoProgram = Program::Create("./shader/defer_geo.vs","./shader/defer_geo.fs");
    m_deferLightProgram = Program::Create("./shader/defer_light.vs", "./shader/defer_light.fs");
    m_blurProgram = Program::Create("./shader/blur_5x5.vs", "./shader/blur_5x5.fs");

    m_deferLights.resize(32);
    for (size_t i = 0; i < m_deferLights.size(); i++)
    {
        m_deferLights[i].position = glm::vec3(
            RandomRange(-10.0f, 10.0f),
            RandomRange(1.0f, 4.0f),
            RandomRange(-10.0f, 10.0f));
        m_deferLights[i].color = glm::vec3(
            RandomRange(0.0f, i > 3 ? 0.0f : 1.0f),
            RandomRange(0.0f, i > 3 ? 0.0f : 1.0f),
            RandomRange(0.0f, i > 3 ? 0.0f : 1.0f));
    }

    //skybox program
    m_skyboxProgram = Program::Create("./shader/skybox.vs", "./shader/skybox.fs");
    if(!m_skyboxProgram){
        spdlog::error("fail to load skyboxProgram");
        return false;
    }
    //envMap program
    m_envMapProgram = Program::Create("./shader/env_map.vs", "./shader/env_map.fs");
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

    m_brickDiffuseTexture = Texture::CreateFromImage(Image::Load("./img/brickwall.jpg", false).get());
    m_brickNormalTexture = Texture::CreateFromImage(Image::Load("./img/brickwall_normal.jpg", false).get());
    int a = 0;
    //brick program
    m_normalProgram = Program::Create("./shader/normal.vs", "./shader/normal.fs");
    if(!m_normalProgram)
    {
        spdlog::error("fail to load normalProgram");
        return false;
    }

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
        Image::Load("./img/marble2.jpg").get());
        m_planeMat->specular = grayTexture;
        m_planeMat->shininess = 4.0f;

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

    m_shadowMap = ShadowMap::Create(1024, 1024);
    m_lightingShadowProgram = Program::Create(
        "./shader/lighting_shadow.vs", "./shader/lighting_shadow.fs");

    std::vector<glm::vec3> ssaoNoise;
    ssaoNoise.resize(16);
    for (size_t i = 0; i < ssaoNoise.size(); i++)
    {
        glm::vec3 sample(RandomRange(-1.0f, 1.0f),RandomRange(-1.0f, 1.0f), 1.0f);
        ssaoNoise[i] = sample;
    }
    m_ssaoNoiseTexture = Texture::Create(4, 4, GL_RGB16F, GL_FLOAT);
    m_ssaoNoiseTexture->Bind();
    m_ssaoNoiseTexture->SetFilter(GL_NEAREST, GL_NEAREST);
    m_ssaoNoiseTexture->SetWrap(GL_REPEAT, GL_REPEAT);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 4, 4, GL_RGB, GL_FLOAT, ssaoNoise.data());
    
    return true;
}


void Context::Render() {
    std::default_random_engine generator;  // 랜덤 넘버 생성기
    std::uniform_real_distribution<float> distribution(0.0f, 3.0f);  // 초당 20도에서 100도 사이의 랜덤 회전 속도
    if (ImGui::Begin("ui window")){
        // light parameters
        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("l.directional", &m_light.directional);
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.5, 0.0f, 3000.0f);
            ImGui::DragFloat("Light Intencity", &m_light.lightIntencity, 0.01f, 0.0f, 4.0f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.5f, 0.0f, 90.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
            ImGui::ColorEdit3("l.shadowColor", glm::value_ptr(m_light.shadowColor));
            ImGui::DragFloat("ssao radius", &m_ssaoRadius, 0.01f, 0.0f, 5.0f);
            ImGui::Checkbox("l.blinn", &m_blinn);
            ImGui::Checkbox("use ssao", &m_useSsao);
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
        //ImGui::Image((ImTextureID)m_shadowMap->GetShadowMap()->Get(), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();

    if (ImGui::Begin("G-Buffers")) 
    {
        const char* bufferNames[] = {"position", "normal", "albedo/specular"};
        static int bufferSelect = 0;
        ImGui::Combo("buffer", &bufferSelect, bufferNames, 3);
        float width = ImGui::GetContentRegionAvail().x;
        float height = width * ((float)m_height / (float)m_width);
        auto selectedAttachment =
        m_deferGeoFramebuffer->GetColorAttachment(bufferSelect);
        ImGui::Image((ImTextureID)selectedAttachment->Get(),
      ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();

    if (ImGui::Begin("SSAO")){
        const char* bufferNames[] = {"original", "blurred"};
        static int bufferSelect = 0;
        ImGui::Combo("ssao buffer", &bufferSelect, bufferNames, 2);
        float width = ImGui::GetContentRegionAvail().x;
        float height = width * ((float)m_height / (float)m_width);
        auto selectedAttachment = bufferSelect == 0 ? 
        m_ssaoFramebuffer->GetColorAttachment() :
        m_blurSsaoFramebuffer->GetColorAttachment();

        ImGui::Image((ImTextureID)selectedAttachment->Get(), 
            ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();



    auto lightView = glm::lookAt(m_light.position, m_light.position + m_light.direction,
        glm::vec3(0.0f, 1.0f, 0.0f));
    auto lightProjection = m_light.directional ? 
        glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 30.0f) :
        glm::perspective(glm::radians(m_light.cutoff[0] + m_light.cutoff[1] * 2.0f), 1.0f, 1.0f, 20.0f);

    m_cameraFront = 
    glm::rotate(glm::mat4(1.0f),
        glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) * 
    glm::rotate(glm::mat4(1.0f),
        glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) * 
    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 100.0f);
    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

    m_shadowMap->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, 
        m_shadowMap->GetShadowMap()->GetWidth(),
        m_shadowMap->GetShadowMap()->GetHeight());
    m_simpleProgram->Use();
    m_simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    DrawScene(lightView, lightProjection, m_simpleProgram.get());

    // defer framebuffer
    m_deferGeoFramebuffer->Bind();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0, m_width, m_height);
    m_deferGeoProgram->Use();
    DrawScene(view, projection, m_deferGeoProgram.get());

    glEnable(GL_DEPTH_TEST);
    Framebuffer::BindToDefault();
    glViewport(0, 0, m_width, m_height);
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_deferLightProgram->Use();
    glActiveTexture(GL_TEXTURE0);
    m_deferGeoFramebuffer->GetColorAttachment(0)->Bind();
    glActiveTexture(GL_TEXTURE1);
    m_deferGeoFramebuffer->GetColorAttachment(1)->Bind();
    glActiveTexture(GL_TEXTURE2);
    m_deferGeoFramebuffer->GetColorAttachment(2)->Bind();
    glActiveTexture(GL_TEXTURE3);
    m_blurSsaoFramebuffer->GetColorAttachment()->Bind();
    glActiveTexture(GL_TEXTURE0);
    m_deferLightProgram->SetUniform("gPosition", 0);
    m_deferLightProgram->SetUniform("gNormal", 1);
    m_deferLightProgram->SetUniform("gAlbedoSpec", 2);
    m_deferLightProgram->SetUniform("ssaoTex", 3);
    m_deferLightProgram->SetUniform("useSsao", m_useSsao ? 1 : 0);
    for (size_t i = 0; i < m_deferLights.size(); i++)
    {
        auto posName = fmt::format("lights[{}].position", i);
        auto colorName = fmt::format("lights[{}].color", i);
        m_deferLightProgram->SetUniform(posName, m_deferLights[i].position);
        m_deferLightProgram->SetUniform(colorName, m_deferLights[i].color);
        m_deferLightProgram->SetUniform("transform", glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
        m_plane->Draw(m_deferLightProgram.get());
    }

    
    m_ssaoFramebuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_width, m_height);
    m_ssaoProgram->Use();
    glActiveTexture(GL_TEXTURE0);
    m_deferGeoFramebuffer->GetColorAttachment(0)->Bind();
    glActiveTexture(GL_TEXTURE1);
    m_deferGeoFramebuffer->GetColorAttachment(1)->Bind();
    glActiveTexture(GL_TEXTURE2);
    m_ssaoNoiseTexture->Bind();
    glActiveTexture(GL_TEXTURE0);
    m_ssaoProgram->SetUniform("gPosition", 0);
    m_ssaoProgram->SetUniform("gNormal", 1);
    m_ssaoProgram->SetUniform("texNoise", 2);
    m_ssaoProgram->SetUniform("noiseScale", glm::vec2(
        (float)m_width / (float)m_ssaoNoiseTexture->GetWidth(), 
        (float)m_height / (float)m_ssaoNoiseTexture->GetHeight()));
    m_ssaoProgram->SetUniform("radius", m_ssaoRadius);
    for (size_t i = 0; i < m_ssaoSamples.size(); i++)
    {
        auto sampleName = fmt::format("samples[{}]", i);
        m_ssaoProgram->SetUniform(sampleName, m_ssaoSamples[i]);
    }
    m_ssaoProgram->SetUniform("transform",
        glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
    m_ssaoProgram->SetUniform("view", view);
    m_ssaoProgram->SetUniform("projection", projection);
    m_plane->Draw(m_ssaoProgram.get());

    m_blurSsaoFramebuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_width, m_height);
    m_blurProgram->Use();
    m_ssaoFramebuffer->GetColorAttachment(0)->Bind();
    m_blurProgram->SetUniform("tex", 0);
    m_blurProgram->SetUniform("transform", 
        glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
    m_plane->Draw(m_ssaoProgram.get());

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_deferGeoFramebuffer->Get());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    m_simpleProgram->Use();
    for (size_t i = 0; i < m_deferLights.size(); i++)
    {
        m_simpleProgram->SetUniform("color", glm::vec4(m_deferLights[i].color, 1.0f));
        m_simpleProgram->SetUniform("transform", projection * view *
            glm::translate(glm::mat4(1.0f), m_deferLights[i].position) * 
            glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)));
        m_box->Draw(m_simpleProgram.get());
    }


    
    // //skybox
    // auto skyboxModelTransform = 
    //     glm::translate(glm::mat4(1.0), m_cameraPos) *
    //     glm::scale(glm::mat4(1.0), glm::vec3(50.0f));
    // m_skyboxProgram->Use();
    // m_cubeTexture->Bind();
    // m_skyboxProgram->SetUniform("skybox", 0);
    // m_skyboxProgram->SetUniform("transform", projection * view * skyboxModelTransform);
    // m_box->Draw(m_skyboxProgram.get());

    // // Light mesh render setting
    // auto lightModelTransform = glm::translate(glm::mat4(1.0), m_light.position) *
    //                            glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
    // m_simpleProgram->Use();
    // m_simpleProgram->SetUniform("color", glm::vec4((m_light.diffuse + m_light.ambient), 1.0f));
    // m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
    // m_box->Draw(m_simpleProgram.get());

    // glm::vec3 lightPos = m_light.position;
    // glm::vec3 lightDir = m_light.direction;
    // if (m_flashLightMode){
    //     lightPos = m_cameraPos;
    //     lightDir = m_cameraFront;
    // }

    // m_lightingShadowProgram->Use();
    // m_lightingShadowProgram->SetUniform("viewPos", m_cameraPos);
    // m_lightingShadowProgram->SetUniform("light.directional", m_light.directional ? 1 : 0);
    // m_lightingShadowProgram->SetUniform("light.position", m_light.position);
    // m_lightingShadowProgram->SetUniform("light.direction", m_light.direction);
    // m_lightingShadowProgram->SetUniform("light.cutoff", glm::vec2(
    //     cosf(glm::radians(m_light.cutoff[0])),
    //     cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    // m_lightingShadowProgram->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    // m_lightingShadowProgram->SetUniform("light.ambient", m_light.ambient);
    // m_lightingShadowProgram->SetUniform("light.diffuse", m_light.diffuse);
    // m_lightingShadowProgram->SetUniform("light.specular", m_light.specular);
    // m_lightingShadowProgram->SetUniform("blinn", (m_blinn ? 1 : 0));
    // m_lightingShadowProgram->SetUniform("lightTransform", lightProjection * lightView);
    // m_lightingShadowProgram->SetUniform("shadowColor", m_light.shadowColor);
    // glActiveTexture(GL_TEXTURE3);
    // m_shadowMap->GetShadowMap()->Bind();
    // m_lightingShadowProgram->SetUniform("shadowMap", 3);
    // glActiveTexture(GL_TEXTURE0);

    // DrawScene(view, projection, m_lightingShadowProgram.get());

    // auto modelTransform = 
    //     glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f)) * 
    //     glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // m_normalProgram->Use();
    // m_normalProgram->SetUniform("viewPos", m_cameraPos);
    // m_normalProgram->SetUniform("lightPos", m_light.position);
    // glActiveTexture(GL_TEXTURE0);
    // m_brickDiffuseTexture->Bind();
    // m_normalProgram->SetUniform("diffuse", 0);
    // glActiveTexture(GL_TEXTURE1);
    // m_brickNormalTexture->Bind();
    // m_normalProgram->SetUniform("normalMap", 1);
    // glActiveTexture(GL_TEXTURE0);
    // m_normalProgram->SetUniform("modelTransform", modelTransform);
    // m_normalProgram->SetUniform("transform", projection * view * modelTransform);
    // m_plane->Draw(m_normalProgram.get());

    //plane
    // auto modelTransform = 
    //     glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) * 
    //     glm::scale(glm::mat4(1.0f), glm::vec3(10.f, 1.0f, 10.0f));
    // auto transform = projection * view * modelTransform;
    // m_program->SetUniform("transform", transform);
    // m_program->SetUniform("modelTransform", modelTransform);
    // m_planeMat->SetToProgram(m_program.get());
    // m_box->Draw(m_program.get());

    //envMap cube
    // modelTransform = 
    //     glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.75f, -2.0f)) * 
    //     glm::rotate(glm::mat4(1.0f), glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
    //     glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    // transform = projection * view * modelTransform;
    // m_envMapProgram->Use();
    // m_envMapProgram->SetUniform("model", modelTransform);
    // m_envMapProgram->SetUniform("view", view);
    // m_envMapProgram->SetUniform("projection", projection);
    // m_envMapProgram->SetUniform("cameraPos", m_cameraPos);
    // m_cubeTexture->Bind();
    // m_envMapProgram->SetUniform("skybox", 0);
    // m_box->Draw(m_envMapProgram.get());

    //box1
    // modelTransform = 
    //     glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
    //     glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f,0.0f)) * 
    //     glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f)); 
    // transform = projection * view * modelTransform;
    // m_program->SetUniform("transform", transform);
    // m_program->SetUniform("modelTransform", modelTransform);
    // m_box1Mat->SetToProgram(m_program.get());
    // m_box->Draw(m_program.get());

    // //box2
    // modelTransform =
    //     glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
    //     glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
    //     glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    // transform = projection * view * modelTransform;
    // m_program->SetUniform("transform", transform);
    // m_program->SetUniform("modelTransform", modelTransform);
    // m_box2Mat->SetToProgram(m_program.get());
    // m_box->Draw(m_program.get());


    //window
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // m_textureProgram->Use();
    // m_windowTexture->Bind();
    // m_textureProgram->SetUniform("tex", 0);

    // modelTransform = 
    //     glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 4.0f));
    // transform = projection * view * modelTransform;
    // m_textureProgram->SetUniform("transform", transform);
    // m_plane->Draw(m_textureProgram.get());

    // modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.5f, 5.0f));
    // transform = projection * view * modelTransform;
    // m_textureProgram->SetUniform("transform", transform);
    // m_plane->Draw(m_textureProgram.get());

    // modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.5f, 6.0f));
    // transform = projection * view * modelTransform;
    // m_textureProgram->SetUniform("transform", transform);
    // m_plane->Draw(m_textureProgram.get());

    //grass
    // glEnable(GL_BLEND);
    // m_grassProgram->Use();
    // m_grassProgram->SetUniform("tex", 0);
    // m_grassTexture->Bind();
    // m_grassInstance->Bind();
    // modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
    // transform = projection * view * modelTransform;
    // m_grassProgram->SetUniform("transform", transform);
    // glDrawElementsInstanced(GL_TRIANGLES,
    //     m_plane->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0,
    // m_grassPosBuffer->GetCount());
    
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

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
    m_framebuffer = Framebuffer::Create({Texture::Create(width, height, GL_RGBA), });
    m_ssaoFramebuffer = Framebuffer::Create({Texture::Create(width, height, GL_RED), });
    m_blurSsaoFramebuffer = Framebuffer::Create({Texture::Create(width, height, GL_RED), });

    m_deferGeoFramebuffer = Framebuffer::Create({
        Texture::Create(width, height, GL_RGBA16F, GL_FLOAT),
        Texture::Create(width, height, GL_RGBA16F, GL_FLOAT),
        Texture::Create(width, height, GL_RGBA16F, GL_UNSIGNED_BYTE)});
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
