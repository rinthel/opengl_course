#include "texture.h"

TextureUPtr Texture::Create(int width, int height, uint32_t format, uint32_t type) {
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFormat(width, height, format, type);
    texture->SetFilter(GL_LINEAR, GL_LINEAR);
    return std::move(texture);
}

TextureUPtr Texture::CreateFromImage(const Image* image) {
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFromImage(image);
    return std::move(texture);
}

Texture::~Texture() {
    if (m_texture) {
        glDeleteTextures(1, &m_texture);
    }
}

void Texture::Bind() const {
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::SetFilter(uint32_t minFilter, uint32_t magFilter) const {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::SetWrap(uint32_t sWrap, uint32_t tWrap) const {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}

void Texture::SetBorderColor(const glm::vec4& color) const {
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
}
    
void Texture::CreateTexture() {
    glGenTextures(1, &m_texture);
    // bind and set default filter and wrap option
    Bind();
    SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::SetTextureFromImage(const Image* image) {
    GLenum format = GL_RGBA;
    switch (image->GetChannelCount()) {
        default: break;
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
    }

    m_width = image->GetWidth();
    m_height = image->GetHeight();
    m_format = format;
    m_type = GL_UNSIGNED_BYTE;
    if (image->GetBytePerChannel() == 4) {
        m_type = GL_FLOAT;
        switch (image->GetChannelCount()) {
            default: break;
            case 1: m_format = GL_R16F; break;
            case 2: m_format = GL_RG16F; break;
            case 3: m_format = GL_RGB16F; break;
            case 4: m_format = GL_RGBA16F; break;
        }
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, m_format,
        m_width, m_height, 0,
        format, m_type,
        image->GetData());

    glGenerateMipmap(GL_TEXTURE_2D);
}

static GLenum GetImageFormat(uint32_t internalFormat) {
    GLenum imageFormat = GL_RGBA;
    if (internalFormat == GL_DEPTH_COMPONENT) {
        imageFormat = GL_DEPTH_COMPONENT;        
    }
    else if (internalFormat == GL_RGB ||
        internalFormat == GL_RGB16F ||
        internalFormat == GL_RGB32F) {
        imageFormat = GL_RGB;
    }
    else if (internalFormat == GL_RG ||
        internalFormat == GL_RG16F ||
        internalFormat == GL_RG32F) {
        imageFormat = GL_RG;
    }
    else if (internalFormat == GL_RED ||
        internalFormat == GL_R ||
        internalFormat == GL_R16F ||
        internalFormat == GL_R32F) {
        imageFormat = GL_RED;
    }
    return imageFormat;
}

void Texture::SetTextureFormat(int width, int height, uint32_t format, uint32_t type) {
    m_width = width;
    m_height = height;
    m_format = format;
    m_type = type;

    GLenum imageFormat = GetImageFormat(m_format);
    glTexImage2D(GL_TEXTURE_2D, 0, m_format,
        m_width, m_height, 0,
        imageFormat, m_type,
        nullptr);
}

CubeTextureUPtr CubeTexture::CreateFromImages(const std::vector<Image*> images) {
    auto texture = CubeTextureUPtr(new CubeTexture());
    if (!texture->InitFromImages(images))
        return nullptr;
    return std::move(texture);
}

CubeTextureUPtr CubeTexture::Create(int width, int height,
    uint32_t format, uint32_t type) {
    auto texture = CubeTextureUPtr(new CubeTexture());
    texture->Init(width, height, format, type);
    return std::move(texture);
}

CubeTexture::~CubeTexture() {
    if (m_texture) {
        glDeleteTextures(1, &m_texture);
    }
}

void CubeTexture::Bind() const {
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);    
}

bool CubeTexture::InitFromImages(const std::vector<Image*> images) {
    glGenTextures(1, &m_texture);
    Bind();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_width = images[0]->GetWidth();
    m_height = images[0]->GetHeight();
    m_type = images[0]->GetBytePerChannel() == 4 ? GL_FLOAT : GL_UNSIGNED_BYTE;
    m_format = images[0]->GetBytePerChannel() == 4 ? GL_RGB16F : GL_RGB;

    for (uint32_t i = 0; i < (uint32_t)images.size(); i++) {
        auto image = images[i];
        GLenum format = GL_RGBA;
        switch (image->GetChannelCount()) {
            default: break;
            case 1: format = GL_RED; break;
            case 2: format = GL_RG; break;
            case 3: format = GL_RGB; break;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_format,
            m_width, m_height, 0, format, m_type, image->GetData());
    }

    return true;
}

void CubeTexture::Init(int width, int height, int format, uint32_t type) {
    glGenTextures(1, &m_texture);
    Bind();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    m_width = width;
    m_height = height;
    m_type = type;
    m_format = format;
    GLenum imageFormat = GetImageFormat(m_format);

    for (uint32_t i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_format,
            m_width, m_height, 0, imageFormat, m_type, nullptr);
    }
}

void CubeTexture::GenerateMipmap() const {
    Bind();
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}