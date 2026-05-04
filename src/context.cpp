#include "context.h"
#include "image.h"
#include <imgui.h>

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if ( !context->Init() )
    {
        return nullptr;
    }
    return std::move(context);
}

void Context::Render()
{
    if ( ImGui::Begin("UI Window") )
    {
        if ( ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen) )
        {
            ImGui::DragFloat3("light.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("light.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("light.cutoff", glm::value_ptr(m_light.cutoff), 0.1f, 0.0f, 180.0f);
            ImGui::DragFloat("light.distance", &m_light.distance, 0.1f, 0.0f, 1000.0f);
            ImGui::ColorEdit3("light.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("light.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("light.specular", glm::value_ptr(m_light.specular));
        }

        ImGui::Separator();

        // if ( ImGui::CollapsingHeader("material", ImGuiTreeNodeFlags_DefaultOpen) )
        // {
        //     ImGui::ColorEdit3("material.baseColor", glm::value_ptr(m_material->baseColor));
        //     ImGui::DragFloat("material.shininess", &m_material->shininess, 1.0f, 1.0f, 256.0f);
        // }

        ImGui::Separator();

        ImGui::Checkbox("Animation", &m_animation);

        ImGui::Separator();

        if ( ImGui::ColorEdit4("Clear color", glm::value_ptr(m_clearColor)) )
        {
            glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        }

        ImGui::Separator();

        ImGui::DragFloat3("Camera Position", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("Camera Yaw", &m_cameraYaw, 0.1f);
        ImGui::DragFloat("Camera Pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);

        ImGui::Separator();

        if ( ImGui::Button("Reset Camera") )
        {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }
    }
    ImGui::End();

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
        100.0f);

    /*
    glm::lockAt을 쓰지 않는다면 이런 식으로 구현할 수 있다.
    auto cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    auto cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    auto cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
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

    // 라이트의 위치 흰 정육면체 그리기
    auto lightModelTransform =
        glm::translate(glm::mat4(1.0), m_light.position) *
        glm::scale(glm::mat4(1.0), glm::vec3(0.1f));

    m_simpleProgram->Use();
    m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
    m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
    m_box->Draw(m_simpleProgram.get());

    m_program->Use();
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", m_light.position);
    m_program->SetUniform("light.direction", m_light.direction);
    m_program->SetUniform(
        "light.cutoff",
        glm::vec2(
            cosf(glm::radians(m_light.cutoff[0])),
            cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))
        )
    );
    m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);

    auto modelTransform = glm::mat4(1.0f);
    auto transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_model->Draw(m_program.get());

    // glDrawElements(primitive, count, type, pointer/offset)
    // - 현재 바인딩된 VAO, VBO, EBO를 바탕으로 그리기
    // - primitive: 그려낼 기본 primitive 타입
    // - count: 그리고자 하는 EBO 내 index의 개수
    // - type: index의 데이터형
    // - pointer/offset: 그리고자 하는 EBO의 첫 데이터로부터의 오프셋
    // glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );
}

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
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

bool Context::Init()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

    m_simpleProgram = Program::Create("./shader/simple.vert", "./shader/simple.frag");
    if ( !m_simpleProgram )
    {
        return false;
    }
    SPDLOG_INFO("simpleProgram id: {}", m_simpleProgram->Get());

    m_program = Program::Create("./shader/lighting.vert", "./shader/lighting.frag");
    if ( !m_program )
    {
        return false;
    }
    SPDLOG_INFO("program id: {}", m_program->Get());

    m_box = Mesh::CreateBox();

    m_model = Model::Load("./model/backpack/backpack.obj");
    if ( !m_model )
    {
        return false;
    }

    return true;
}
