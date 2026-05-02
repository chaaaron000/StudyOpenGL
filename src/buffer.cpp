#include "buffer.h"

BufferUPtr Buffer::CreateWithData(uint32_t bufferType, uint32_t usage, const void* data, size_t stride, size_t count)
{
    auto buffer = BufferUPtr(new Buffer());
    if ( !buffer->Init(bufferType, usage, data, stride, count) )
    {
        return nullptr;
    }
    return std::move(buffer);
}

Buffer::~Buffer()
{
    if ( m_buffer )
    {
        glDeleteBuffers(1, &m_buffer);
    }
}

void Buffer::Bind() const
{
    // 지금부터 사용할 buffer object를 지정한다
    // GL_ARRAY_BUFFER: 사용할 buffer object는 vertex data를 저장할 용도임을 알려줌
    glBindBuffer(m_bufferType, m_buffer);
}

bool Buffer::Init(uint32_t bufferType, uint32_t usage, const void* data, size_t stride, size_t count)
{
    m_bufferType = bufferType;
    m_usage = usage;
    m_stride = stride;
    m_count = count;

    // 새로운 buffer object를 만든다
    glGenBuffers(1, &m_buffer);

    Bind();

    // 지정된 buffer에 데이터를 복사한다
    //  - 데이터의 총 크기, 데이터 포인터, 용도를 지정
    //  - 용도는 "STATIC | DYNAMIC | STREAM", "DRAW | COPY | READ"의 조합
    //  - flag 설명
    //    - GL_STATIC_DRAW: 딱 한번만 세팅되고 앞으로 계속 쓸 예정
    //    - GL_DYNAMIC_DRAW: 앞으로 데이터가 자주 바뀔 예정
    //    - GL_STREAM_DRAW: 딱 한번만 세팅되고 몇번 쓰다 버려질 예정
    //    - 용도에 맞는 flag를 지정해야 효율이 올라감
    glBufferData(m_bufferType, m_stride * m_count, data, m_usage);

    return true;
}
