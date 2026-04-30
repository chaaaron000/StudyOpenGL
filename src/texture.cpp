#include "texture.h"

TextureUPtr Texture::CreateFromImage( const Image* image )
{
    auto texture = TextureUPtr( new Texture() );
    texture->CreateTexture();
    texture->SetTextureFromImage( image );
    return std::move( texture );
}

Texture::~Texture()
{
    if ( m_texture )
    {
        glDeleteTextures( 1, &m_texture );
    }
}

void Texture::Bind() const
{
    glBindTexture( GL_TEXTURE_2D, m_texture );
}

void Texture::SetFilter( uint32_t minFilter, uint32_t magFilter ) const
{
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter );
}

void Texture::SetWrap( uint32_t sWrap, uint32_t tWrap ) const
{
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap );
}

void Texture::CreateTexture()
{
    glGenTextures( 1, &m_texture );
    
    // bind and set default filter and wrap option
    Bind();
    
    // - GL_NEAREST_MIPMAP_NEAREST: 적합한 레벨의 텍스처를 선택한 뒤 nearest neighbor 픽셀을 선택한다
    // - GL_LINEAR_MIPMAP_LINEAR: 적합한 두 레벨의 텍스처에서 linear filtering된 값을 다시 linear interpolation한다 (trilinear interpolation)
    SetFilter( GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR );
    SetWrap( GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE );
}

void Texture::SetTextureFromImage( const Image* image )
{
    GLenum format = GL_RGBA;
    switch ( image->GetChannelCount() )
    {
    default:
        break;
    case 1:
        format = GL_RED;
        break;
    case 2:
        format = GL_RG;
        break;
    case 3:
        format = GL_RGB;
        break;
    }

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, image->GetWidth(), image->GetHeight(), 0, format, GL_UNSIGNED_BYTE,
                  image->GetData() );

    glGenerateMipmap( GL_TEXTURE_2D );
}
