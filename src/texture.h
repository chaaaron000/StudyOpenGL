#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "image.h"

CLASS_PTR( Texture )
class Texture
{
public:
    static TextureUPtr CreateFromImage( const Image* image );

    ~Texture();

    const uint32_t Get() const
    {
        return m_texture;
    }

    void Bind() const;
    void SetFilter( uint32_t minFilter, uint32_t magFilter ) const;
    void SetWrap( uint32_t sWrap, uint32_t tWrap ) const;

private:
    Texture()
    {
    }

    void CreateTexture();

    // 왜 ImagePtr이나 ImageUPtr이 아닌 Image*를 인자로 쓰는가?
    // - ImageUPtr: 이미지 인스턴스 소유권이 함수 안으로 넘어오게 됨
    // - ImagePtr: 이미지 인스턴스 소유권을 공유함
    // - Image*: 소유권과 상관없이 인스턴스에 접근
    void SetTextureFromImage( const Image* image );

    uint32_t m_texture{ 0 };
};

#endif //__TEXTURE_H__
