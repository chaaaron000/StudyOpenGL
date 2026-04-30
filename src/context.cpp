#include "context.h"
#include "image.h"

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if ( !context->Init() )
    {
        return nullptr;
    }
    return std::move(context);
}

bool Context::Init()
{
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // x, y, z, u, v
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
    };

    uint32_t indices[] = {
        0, 2, 1, 2, 0, 3, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11, 8,
        12, 14, 13, 14, 12, 15, 16, 17, 18, 18, 19, 16, 20, 22, 21, 22, 20, 23,
    };

    // 순서 주의
    // vertex attribute을 설정하기 전에 VBO가 바인딩 되어있을 것
    // 1. VAO binding
    // 2. VBO binding
    // 3. vertex attribute setting

    // 1. VAO binding
    m_vertexLayout = VertexLayout::Create();

    // 2. VBO binding
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 120);

    // 3. vertex attribute setting
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0); // 0: position attribute
    // m_vertexLayout->SetAttrib( 1, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 8,
    //                            sizeof( float ) * 3 ); // 1: color attribute
    m_vertexLayout->SetAttrib(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(float) * 5,
        sizeof(float) * 3); // 2: texture uv attribute

    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 36);

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/texture.vert", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/texture.frag", GL_FRAGMENT_SHADER);
    if ( !vertShader || !fragShader )
    {
        return false;
    }
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());
    m_program = Program::Create({ fragShader, vertShader });
    if ( !m_program )
    {
        return false;
    }
    SPDLOG_INFO("program id: {}", m_program->Get());

    auto loc = glGetUniformLocation(m_program->Get(), "color");
    m_program->Use();
    glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f);

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    // 이미지 로딩
    auto image = Image::Load("./image/container.jpg");
    if ( !image )
    {
        return false;
    }
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());

    // auto image = Image::Create( 512, 512 );
    // image->SetCheckImage( 16, 16 );

    // 텍스처 생성
    m_texture = Texture::CreateFromImage(image.get());

    auto image2 = Image::Load("./image/awesomeface.png");
    m_texture2 = Texture::CreateFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());
    m_program->Use();
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);

    // x축으로 -55도 회전
    auto model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    // 카메라는 원점으로부터 z축 방향으로 -3만큼 떨어짐
    auto view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    // 종횡비 4:3, 세로화각 45도의 원근 투영
    auto projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(640) / static_cast<float>(480),
        0.01f,
        10.0f);
    // auto projection = glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f, 0.01f, 10.0f );
    auto transform = projection * view * model;
    auto transformLoc = glGetUniformLocation(m_program->Get(), "transform");
    m_program->SetUniform("transform", transform);

    return true;
}

void Context::Render()
{
    std::vector<glm::vec3> cubePositions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // glDrawArray(primitive, offset, count)
    // - 현재 설정된 program, VBO, VAO로 그림을 그린다
    // - primitive: 그리고자 하는 primitive 타입
    // - offset: 그리고자 하는 첫 정점의 index
    // - count: 그리려는 정점의 총 개수
    // glDrawArrays( GL_TRIANGLES, 0, 6 );

    m_cameraFront =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    // auto projection = glm::ortho( -1.0f, 1.0f, -1.0f, 1.0f, 0.01f, 10.0f );
    auto projection = glm::perspective(
        glm::radians(45.0f),
        static_cast<float>(m_width) / static_cast<float>(m_height),
        0.01f,
        10.0f);

    /*
    glm::lockAt을 쓰지 않는다면 이런 식으로 구현할 수 있다.
    auto cameraZ = glm::normalize(cameraPos - cameraTarget);
    auto cameraX = glm::normalize(glm::cross(cameraUp, cameraZ));
    auto cameraY = glm::cross(cameraZ, cameraX);
    auto cameraMat = glm::mat4(
      glm::vec4(cameraX, 0.0f),
      glm::vec4(cameraY, 0.0f),
      glm::vec4(cameraZ, 0.0f),
      glm::vec4(cameraPos, 1.0f));
    auto view = glm::inverse(cameraMat);
    */
    auto view = glm::lookAt(
        m_cameraPos,
        m_cameraPos + m_cameraFront,
        m_cameraUp);

    auto cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    auto cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    auto cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    for ( size_t i = 0; i < cubePositions.size(); i++ )
    {
        auto& pos = cubePositions[i];
        auto model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::rotate(
            model,
            glm::radians(
                static_cast<float>(glfwGetTime()) * 120.0f + 20.0f * static_cast<float>(
                    i)),
            glm::vec3(1.0f, 0.5f, 0.0f));
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }


    // glDrawElements(primitive, count, type, pointer/offset)
    // - 현재 바인딩된 VAO, VBO, EBO를 바탕으로 그리기
    // - primitive: 그려낼 기본 primitive 타입
    // - count: 그리고자 하는 EBO 내 index의 개수
    // - type: index의 데이터형
    // - pointer/offset: 그리고자 하는 EBO의 첫 데이터로부터의 오프셋
    // glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );
}

void Context::ProcessInput(GLFWwindow* window)
{
    if ( !m_cameraControl )
    {
        return;
    }
    const float cameraSpeed = 0.05f;

    if ( glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS )
    {
        m_cameraPos += cameraSpeed * m_cameraFront;
    }
    if ( glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS )
    {
        m_cameraPos -= cameraSpeed * m_cameraFront;
    }

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if ( glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS )
    {
        m_cameraPos += cameraSpeed * cameraRight;
    }
    if ( glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS )
    {
        m_cameraPos -= cameraSpeed * cameraRight;
    }

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if ( glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS )
    {
        m_cameraPos += cameraSpeed * cameraUp;
    }
    if ( glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS )
    {
        m_cameraPos -= cameraSpeed * cameraUp;
    }
}

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y)
{
    if ( !m_cameraControl )
    {
        return;
    }
    auto pos = glm::vec2(static_cast<float>(x), static_cast<float>(y));
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.8f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if ( m_cameraYaw < 0.0f )
    {
        m_cameraYaw += 360.0f;
    }
    if ( m_cameraYaw > 360.0f )
    {
        m_cameraYaw -= 360.0f;
    }

    if ( m_cameraPitch > 89.0f )
    {
        m_cameraPitch = 89.0f;
    }
    if ( m_cameraPitch < -89.0f )
    {
        m_cameraPitch = -89.0f;
    }
    
    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y)
{
    if ( button == GLFW_MOUSE_BUTTON_RIGHT )
    {
        if ( action == GLFW_PRESS )
        {
            // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
            m_prevMousePos = glm::vec2(static_cast<float>(x), static_cast<float>(y));
            m_cameraControl = true;
        }
        else if ( action == GLFW_RELEASE )
        {
            m_cameraControl = false;
        }
    }
}
