#ifndef __SHADOW_MAP_H__
#define __SHADOW_MAP_H__

#include "texture.h"

CLASS_PTR(ShadowMap);
class ShadowMap {
public:
    static ShadowMapUPtr Create(int width, int height);
    ~ShadowMap();

    const uint32_t Get() const { return m_framebuffer; }
    void Bind() const;
    const TexturePtr GetShadowMap() const { return m_shadowMap; }

private:
    ShadowMap() {}
    bool Init(int width, int height);

    uint32_t m_framebuffer { 0 };
    TexturePtr m_shadowMap;
};

#endif // __SHADOW_MAP_H__