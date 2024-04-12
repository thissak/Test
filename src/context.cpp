#include "context.h"

ContextUPtr Context::Create(){
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}
    

bool Context::Init(){
    ShaderPtr vertShader = Shader::CreateFromFile("./shader/simpleShader.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/simpleShader.fs", GL_FRAGMENT_SHADER);
    if(!vertShader || !fragShader)
        return false;
    spdlog::info("vertex shader id: {}", vertShader->Get());
    spdlog::info("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});
    if(!m_program)
        return false;
    spdlog::info("program id: {}", m_program->Get());

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    uint32_t vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    return true;
}

void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_program->Get());
    glDrawArrays(GL_POINTS, 0, 1);
}
