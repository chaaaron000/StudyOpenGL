#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

ImageUPtr Image::Load(const std::string& filepath)
{
    auto image = ImageUPtr(new Image());
    if ( !image->LoadWithStb(filepath) )
    {
        return nullptr;
    }
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());
    return std::move(image);
}

ImageUPtr Image::Create(int width, int height, int channelCount)
{
    auto image = ImageUPtr(new Image());
    if ( !image->Allocate(width, height, channelCount) )
    {
        return nullptr;
    }
    return std::move(image);
}

ImageUPtr Image::CreateSingleColorImage(int width, int height, const glm::vec4& color)
{
    glm::vec4 clamped = glm::clamp(color * 255.0f, 0.0f, 255.0f);
    uint8_t rgba[4] = {
        (uint8_t)clamped.r,
        (uint8_t)clamped.g,
        (uint8_t)clamped.b,
        (uint8_t)clamped.a,
    };
    auto image = Create(width, height, 4);
    for ( int i = 0; i < width * height; i++ )
    {
        memcpy(image->m_data + 4 * i, rgba, 4);
    }
    return std::move(image);
}

Image::~Image()
{
    if ( m_data )
    {
        stbi_image_free(m_data);
    }
}

void Image::SetCheckImage(int gridX, int gridY)
{
    // gridX, gridY 크기의 흑백 타일로 구성된 체커 보드 이미지
    // 알파 체널은 항상 255로 설정

    for ( int v = 0; v < m_height; ++v )
    {
        for ( int u = 0; u < m_width; ++u )
        {
            int pos = ( v * m_width + u ) * m_channelCount;
            bool even = ( ( u / gridX ) + ( v / gridY ) ) % 2 == 0;
            uint8_t value = even ? 255 : 0;

            for ( int c = 0; c < m_channelCount; ++c )
            {
                m_data[pos + c] = value;
            }

            if ( m_channelCount > 3 )
            {
                m_data[pos + 3] = 255;
            }
        }
    }
}

bool Image::LoadWithStb(const std::string& filepath)
{
    stbi_set_flip_vertically_on_load(true);
    m_data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
    if ( !m_data )
    {
        SPDLOG_ERROR("failed to load image: {}", filepath);
        return false;
    }
    return true;
}

bool Image::Allocate(int width, int height, int channelCount)
{
    m_width = width;
    m_height = height;
    m_channelCount = channelCount;
    m_data = static_cast<uint8_t*>(malloc(m_width * m_height * m_channelCount));
    return m_data ? true : false;
}
