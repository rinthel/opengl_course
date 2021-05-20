#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "image.h"

CLASS_PTR(Texture)
class Texture {
public:
    static TextureUPtr Create(int width, int height, uint32_t format, uint32_t type = GL_UNSIGNED_BYTE);
    static TextureUPtr CreateFromImage(const Image* image);
    ~Texture();

    const uint32_t Get() const { return m_texture; }
    void Bind() const;
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;
    void SetBorderColor(const glm::vec4& color) const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    uint32_t GetFormat() const { return m_format; }
    uint32_t GetType() const { return m_type; }

private:
    Texture() {}
    void CreateTexture();
    void SetTextureFromImage(const Image* image);
    void SetTextureFormat(int width, int height, uint32_t format, uint32_t type);

    uint32_t m_texture { 0 };
    int m_width { 0 };
    int m_height { 0 };
    uint32_t m_format { GL_RGBA };
    uint32_t m_type { GL_UNSIGNED_BYTE };
};

CLASS_PTR(CubeTexture)
class CubeTexture {
public:
    static CubeTextureUPtr CreateFromImages(const std::vector<Image*> images);
    ~CubeTexture();

    const uint32_t Get() const { return m_texture; }
    void Bind() const;

private:
    CubeTexture() {}
    bool InitFromImages(const std::vector<Image*> images);

    uint32_t m_texture { 0 };
};

#endif // __TEXTURE_H__