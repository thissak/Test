#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertexLayout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "framebuffer.h"
#include "shadow_map.h"

CLASS_PTR(Context)
class Context{
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);

    void DrawScene(const glm::mat4& view, const glm::mat4& projection, 
        const Program* program);
private:
    Context(){}
    bool Init();
    ProgramUPtr m_program;
    ProgramUPtr m_simpleProgram;
    ProgramUPtr m_textureProgram;
    ProgramUPtr m_postProgram;
    float m_gamma { 1.0f };

    MeshUPtr m_box;
    MeshUPtr m_plane;

    MaterialPtr m_planeMat;
    MaterialPtr m_box1Mat;
    MaterialPtr m_box2Mat;
    TexturePtr m_windowTexture;
    
    //camera parameter
    bool m_cameraControl { false };
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    float m_cameraPitch { -20.0f };
    float m_cameraYaw { 0.0f };
    glm::vec3 m_cameraPos{ glm::vec3(-0.17f, 3.95f, 1.7f)};
    glm::vec3 m_cameraFront{ glm::vec3(0.0f, -1.0f, 0.0f)};
    glm::vec3 m_cameraUp{ glm::vec3(0.0f, 1.0f, 0.0f)};

    // animation
    bool m_animation { true };

    //clear color
    glm::vec4 m_clearColor { glm::vec4(1.0f, 0.2f, 0.3f, 0.0f) };

    // light parameter
    struct Light 
    {   bool directional { false };
        glm::vec3 position { glm::vec3(0.0f, 3.3f, 0.0f) };
        glm::vec3 direction { glm::vec3(-0.5f, -1.5f, -1.0f) };
        glm::vec2 cutoff { glm::vec2(50.0f, 5.0f) };
        float distance { 150.0f };
        float lightIntencity { 1.0f };
        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(1.0f, 1.0f, 1.0f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
        glm::vec3 shadowColor { glm::vec3(1.0f, 0.0f, 0.0f)};
    };
    Light m_light;
    bool m_flashLightMode { false };
    bool m_blinn { true };

    //framebuffer
    FramebufferUPtr m_framebuffer;

    //normal map
    TexturePtr m_brickDiffuseTexture;
    TexturePtr m_brickNormalTexture;
    ProgramUPtr m_normalProgram;

    //cubemap
    CubeTextureUPtr m_cubeTexture;
    ProgramUPtr m_skyboxProgram;
    ProgramUPtr m_envMapProgram;

    //grass
    TexturePtr m_grassTexture;
    ProgramUPtr m_grassProgram;
    std::vector<glm::vec3> m_grassPos;
    BufferUPtr m_grassPosBuffer;
    VertexLayoutUPtr m_grassInstance;

    // shadow map
    ShadowMapUPtr m_shadowMap;
    ProgramUPtr m_lightingShadowProgram;

    int m_width { WINDOW_WIDTH };
    int m_height { WINDOW_HEIGHT };


};

#endif __CONTEXT_H__