

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "common.h"
#include "shader.h"

/*
 * vertex, fragment shader 외에 여러 개의 Shader를 링크할 수도 있게 함
 * Shader 인스턴스 인자는 필요하지만 소유할 필요는 없음
 * Shader 인스턴스는 다른 Program 인스턴스를 만드는 데 재사용할 수도 있음
 * 따라서 shared pointer를 사용: ShaderPtr
 */
CLASS_PTR( Program )
class Program
{
public:
    static ProgramUPtr Create( const std::vector<ShaderPtr> &shaders );

    ~Program();
    
    [[nodiscard]] uint32_t Get() const
    {
        return m_program;
    }
    
    void Use() const;

private:
    Program()
    {
    }
    
    bool Link( const std::vector<ShaderPtr> &shaders );
    
    uint32_t m_program{ 0 };
};

#endif //__PROGRAM_H__
