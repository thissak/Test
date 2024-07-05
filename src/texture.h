#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "common.h"
#include "image.h"

CLASS_PTR(Texture)
class Texture{
public:
    static TextureUPtr Create(int width, int height, 
        uint32_t format, uint32_t type = GL_UNSIGNED_BYTE);
    ~Texture();
    uint32_t GetType() const { return m_type; }
    uint32_t m_type { GL_UNSIGNED_BYTE };
    void SetTextureFormat(int width, int height, uint32_t format, uint32_t type);
    static TextureUPtr CreateFromImage(const Image* image);

    const uint32_t Get() const { return m_texture; }
    void Bind() const;
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;
    void SetBorderColor(const glm::vec4& color) const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    uint32_t GetFormat() const { return m_format; }

private:
    Texture() {}
    void CreateTexture();
    void SetTextureFromImage(const Image* image);

    uint32_t m_texture { 0 };
    int m_width { 0 };
    int m_height { 0 };
    uint32_t m_format { GL_RGBA };
};

CLASS_PTR(CubeTexture)
class CubeTexture{
public:
    static CubeTextureUPtr CreateFromImages(const std::vector<Image*>& images);
    static CubeTextureUPtr Create(int width, int height, uint32_t format, uint32_t type = GL_UNSIGNED_BYTE);
    ~CubeTexture();

    const uint32_t Get() { return m_texture; }
    void Bind() const;

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    uint32_t GetFormat() const { return m_format; }
    uint32_t GetType() const { return m_type; }

private:
    CubeTexture() {}
    bool InitFromImages(const std::vector<Image*>& images);
    void Init(int width, int height, int format, uint32_t type);

    uint32_t m_texture { 0 };
    int m_width { 0 };
    int m_height { 0 };
    uint32_t m_format { GL_RGBA };
    uint32_t m_type { GL_UNSIGNED_BYTE };
};

#endif //_TEXTURE_H__