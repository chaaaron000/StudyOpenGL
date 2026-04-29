#include "shader.h"

ShaderUPtr Shader::CreateFromFile( const std::string &filename, const GLenum shaderType )
{
    auto shader = std::unique_ptr<Shader>( new Shader() );
    if ( !shader->LoadFile( filename, shaderType ) )
    {
        return nullptr;
    }
    return std::move( shader );
}

bool Shader::LoadFile( const std::string &filename, const GLenum shaderType )
{
    const auto result = LoadTextFile( filename );

    // 파일 로딩 실패시 false 리턴
    if ( !result.has_value() )
    {
        return false;
    }

    // 성공시 로딩된 텍스트 포인터 및 길이 가져오기
    const auto &code = result.value();
    const char *codePtr = code.c_str();
    const auto codeLength = static_cast<int32_t>( code.length() );
    
    // shader 오브젝트 생성
    m_shader = glCreateShader( shaderType );

    // 소스코드 입력
    glShaderSource( m_shader, 1, static_cast<const GLchar *const *>( &codePtr ), &codeLength );

    // shader 컴파일
    glCompileShader( m_shader );

    // check compile error
    int success = 0;
    glGetShaderiv( m_shader, GL_COMPILE_STATUS, &success ); // 컴파일 상태 조회
    if ( !success )
    {
        char infoLog[1024];
        glGetShaderInfoLog( m_shader, 1024, nullptr, infoLog ); // 에러 로그 가져오기
        SPDLOG_ERROR( "failed to compile shader: \"{}\"", filename );
        SPDLOG_ERROR( "reason: {}", infoLog );
        return false;
    }
    
    return true;
}

Shader::~Shader()
{
    // m_shader는 처음에 0으로 초기화
    // m_shader에 0이 아닌 다른 값이 들어가 있다면 glDeleteShader()를 호출하여 shader object 제거
    if ( m_shader )
    {
        glDeleteShader( m_shader );
    }
}