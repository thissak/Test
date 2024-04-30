#include "framebuffer.h"

FramebufferUPtr Framebuffer::Create(const TexturePtr colorAttachment){
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    int a = 0;
    if (!framebuffer->InitWithColorAttachment(colorAttachment))
        return nullptr;
    return std::move(framebuffer);
}

Framebuffer::~Framebuffer(){
    if (m_framebuffer)
        glDeleteFramebuffers(1, &m_framebuffer);
    if (m_depthStencilBuffer)
        glDeleteRenderbuffers(1, &m_depthStencilBuffer);
}

void Framebuffer::BindToDefault(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool Framebuffer::InitWithColorAttachment(const TexturePtr colorAttachment){
    m_colorAttachment = colorAttachment;
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
        GL_TEXTURE_2D, colorAttachment->Get(), 0);

    glGenRenderbuffers(1, &m_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 
        colorAttachment->GetWidth(), colorAttachment->GetHeight());
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 
        GL_RENDERBUFFER, m_depthStencilBuffer);

    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (result != GL_FRAMEBUFFER_COMPLETE){
        spdlog::error("fail to create framebuffer {}", result);
        return false;
    }

    BindToDefault();
    
    return true;
}