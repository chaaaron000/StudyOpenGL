#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "common.h"

CLASS_PTR( Image )
class Image
{
public:
    static ImageUPtr Load( const std::string &filepath );
    static ImageUPtr Create( int width, int height, int channelCount = 4 );

    ~Image();

    [[nodiscard]] const uint8_t *GetData() const
    {
        return m_data;
    }

    [[nodiscard]] int GetWidth() const
    {
        return m_width;
    }

    [[nodiscard]] int GetHeight() const
    {
        return m_height;
    }

    [[nodiscard]] int GetChannelCount() const
    {
        return m_channelCount;
    }

    void SetCheckImage( int gridX, int gridY );

private:
    Image() {};

    bool LoadWithStb( const std::string &filepath );
    bool Allocate( int width, int height, int channelCount );

    int m_width{ 0 };
    int m_height{ 0 };
    int m_channelCount{ 0 };
    uint8_t *m_data{ nullptr };
};

#endif //__IMAGE_H__
