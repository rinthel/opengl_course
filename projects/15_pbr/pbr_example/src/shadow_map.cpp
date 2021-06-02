#include "shadow_map.h"

ShadowMapUPtr ShadowMap::Create(int width, int height) {
    auto shadowMap = ShadowMapUPtr(new ShadowMap());
    if (!shadowMap->Init(width, height))
        return nullptr;
    return std::move(shadowMap);
}

ShadowMap::~ShadowMap() {
    if (m_framebuffer) {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}

void ShadowMap::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool ShadowMap::Init(int width, int height) {
    glGenFramebuffers(1, &m_framebuffer);
    Bind();

    m_shadowMap = Texture::Create(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
    m_shadowMap->SetFilter(GL_NEAREST, GL_NEAREST);
    m_shadowMap->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    m_shadowMap->SetBorderColor(glm::vec4(1.0f));

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, m_shadowMap->Get(), 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        SPDLOG_ERROR("failed to complete shadow map framebuffer: {:x}", status);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

