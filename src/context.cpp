#include "context.h"

ContextUPtr Context::Create(){
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}
    

bool Context::Init(){
    float vertices[] = {
        0.5f,  0.5f, 0.0f, // 오른쪽 위
        0.5f, -0.5f, 0.0f, // 오른쪽 아래
        -0.5f, -0.5f, 0.0f, // 왼쪽 아래
        -0.5f,  0.5f, 0.0f, // 왼쪽 위
    };
    uint32_t indices[]={
        0, 1, 3,
        1, 2, 3
    };


    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 12);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);

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
   
    return true;
}

void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(m_program->Get());
    //m_program->Use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
