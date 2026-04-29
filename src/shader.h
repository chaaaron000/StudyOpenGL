//
// Created by User on 26. 4. 29..
//

#ifndef __SHADER_H__
#define __SHADER_H__

#include "common.h"

/*
 * 생성자가 private인 이유: CreateFromFile() 함수 외에 다른 방식의 Shader 인스턴스 생성을 막기 위해서
 * Get()은 있는데 Set()는 없는 이유: shader 오브젝트의 생성 관리는 Shader 내부에서만 관리
 * LoadFile()이 bool을 리턴하는 이유: 생성에 실패할 경우 false를 리턴하기 위해서
 */
CLASS_PTR( Shader );
class Shader
{
public:
    static ShaderUPtr CreateFromFile( const std::string &filename, GLenum shaderType );

    ~Shader();

    [[nodiscard]] uint32_t Get() const
    {
        return m_shader;
    }

private:
    Shader()
    {
    }

    bool LoadFile( const std::string &filename, GLenum shaderType );

    uint32_t m_shader{ 0 };
};

#endif //__SHADER_H__
