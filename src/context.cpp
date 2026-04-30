#include "context.h"
#include "image.h"

ContextUPtr Context::Create()
{
    auto context = ContextUPtr( new Context() );
    if ( !context->Init() )
    {
        return nullptr;
    }
    return std::move( context );
}

bool Context::Init()
{
    float vertices[] = {
        // [x, y, z, r, g, b, s, t]
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, //
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, //
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, //
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, //
    };

    uint32_t indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    // 순서 주의
    // vertex attribute을 설정하기 전에 VBO가 바인딩 되어있을 것
    // 1. VAO binding
    // 2. VBO binding
    // 3. vertex attribute setting

    // 1. VAO binding
    m_vertexLayout = VertexLayout::Create();

    // 2. VBO binding
    m_vertexBuffer = Buffer::CreateWithData( GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof( float ) * 32 );

    // 3. vertex attribute setting
    m_vertexLayout->SetAttrib( 0, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 8, 0 ); // 0: position attribute
    m_vertexLayout->SetAttrib( 1, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 8,
                               sizeof( float ) * 3 ); // 1: color attribute
    m_vertexLayout->SetAttrib( 2, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 8,
                               sizeof( float ) * 6 ); // 2: texture uv attribute

    m_indexBuffer = Buffer::CreateWithData( GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof( uint32_t ) * 6 );

    ShaderPtr vertShader = Shader::CreateFromFile( "./shader/texture.vert", GL_VERTEX_SHADER );
    ShaderPtr fragShader = Shader::CreateFromFile( "./shader/texture.frag", GL_FRAGMENT_SHADER );
    if ( !vertShader || !fragShader )
    {
        return false;
    }
    SPDLOG_INFO( "vertex shader id: {}", vertShader->Get() );
    SPDLOG_INFO( "fragment shader id: {}", fragShader->Get() );
    m_program = Program::Create( { fragShader, vertShader } );
    if ( !m_program )
    {
        return false;
    }
    SPDLOG_INFO( "program id: {}", m_program->Get() );

    auto loc = glGetUniformLocation( m_program->Get(), "color" );
    m_program->Use();
    glUniform4f( loc, 1.0f, 1.0f, 0.0f, 1.0f );

    glClearColor( 0.1f, 0.2f, 0.3f, 0.0f );

    // 이미지 로딩
    auto image = Image::Load( "./image/container.jpg" );
    if ( !image )
    {
        return false;
    }
    SPDLOG_INFO( "image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount() );

    // auto image = Image::Create( 512, 512 );
    // image->SetCheckImage( 16, 16 );

    // 텍스처 생성
    m_texture = Texture::CreateFromImage( image.get() );

    auto image2 = Image::Load( "./image/awesomeface.png" );
    m_texture2 = Texture::CreateFromImage( image2.get() );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_texture->Get() );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, m_texture2->Get() );
    m_program->Use();
    glUniform1i( glGetUniformLocation( m_program->Get(), "tex" ), 0 );
    glUniform1i( glGetUniformLocation( m_program->Get(), "tex2" ), 1 );

    return true;
}

void Context::Render()
{
    glClear( GL_COLOR_BUFFER_BIT );

    static float time = 0.0f;
    float t = sinf( time ) * 0.5f + 0.5f;
    auto loc = glGetUniformLocation( m_program->Get(), "color" );
    m_program->Use();
    glUniform4f( loc, t * t, 2.0f * t * ( 1.0f - t ), ( 1.0f - t ) * ( 1.0f - t ), 1.0f );

    // glDrawArray(primitive, offset, count)
    // - 현재 설정된 program, VBO, VAO로 그림을 그린다
    // - primitive: 그리고자 하는 primitive 타입
    // - offset: 그리고자 하는 첫 정점의 index
    // - count: 그리려는 정점의 총 개수
    // glDrawArrays( GL_TRIANGLES, 0, 6 );

    // glDrawElements(primitive, count, type, pointer/offset)
    // - 현재 바인딩된 VAO, VBO, EBO를 바탕으로 그리기
    // - primitive: 그려낼 기본 primitive 타입
    // - count: 그리고자 하는 EBO 내 index의 개수
    // - type: index의 데이터형
    // - pointer/offset: 그리고자 하는 EBO의 첫 데이터로부터의 오프셋
    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );

    time += 0.016f;
}
