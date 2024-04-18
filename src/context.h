#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertexLayout.h"
#include "texture.h"
#include "mesh.h"

CLASS_PTR(Context)
class Context{
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);
private:
    Context(){}
    bool Init();
    ProgramUPtr m_program;
    ProgramUPtr m_simpleProgram;

    MeshUPtr m_box; 
    TextureUPtr m_texture0;
    TextureUPtr m_texture1;

    //camera parameter
    bool m_cameraControl { false };
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    float m_cameraPitch { 0.0f };
    float m_cameraYaw { 0.0f };
    glm::vec3 m_cameraPos{ glm::vec3(0.0f, 0.0f, 9.0f)};
    glm::vec3 m_cameraFront{ glm::vec3(0.0f, 0.0f, -1.0f)};
    glm::vec3 m_cameraUp{ glm::vec3(0.0f, 1.0f, 0.0f)};

    // animation
    bool m_animation { true };

    //clear color
    glm::vec4 m_clearColor { glm::vec4(1.0f, 0.2f, 0.3f, 0.0f) };

    // light parameter
    struct Light {
        glm::vec3 position { glm::vec3(3.0f, -0.7f, 0.7f) };
        glm::vec3 direction { glm::vec3(-1.0f, -1.0f, -1.0f) };
        glm::vec2 cutoff { glm::vec2(80.0f, 5.0f) };
        float distance { 32.0f };
        glm::vec3 ambient { glm::vec3(0.5f, 0.5f, 0.5f) };
        glm::vec3 diffuse { glm::vec3(1.0f, 1.0f, 1.0f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };
    Light m_light;

    // material parameter
    struct Material {
        TextureUPtr diffuse;
        TextureUPtr specular;
        float shininess { 32.0f };
    };
    Material m_material;

    int m_width { WINDOW_WIDTH };
    int m_height { WINDOW_HEIGHT };


};

#endif __CONTEXT_H__