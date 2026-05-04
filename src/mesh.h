#ifndef __MESH_H__
#define __MESH_H__

#include "common.h"
#include "buffer.h"
#include <EASTL/vector.h>
#include "texture.h"
#include "vertex_layout.h"


class Program;


struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};


CLASS_PTR(Material);
CLASS_PTR(Mesh);


class Material
{
public:
    static MaterialUPtr Create() { return MaterialUPtr(new Material()); }

    void SetToProgram(const Program* program) const;

    TexturePtr diffuse;
    TexturePtr specular;
    glm::vec3 baseColor { glm::vec3(1.0f, 1.0f, 1.0f) };
    float shininess { 32.0f };

private:
    Material()
    {
    }
};


class Mesh
{
public:
    static MeshUPtr Create(
        const eastl::vector<Vertex>& vertices,
        const eastl::vector<uint32_t>& indices,
        uint32_t primitiveType);
    static MeshUPtr CreateBox();

    [[nodiscard]] const VertexLayout* GetVertexLayout() const { return m_vertexLayout.get(); }
    [[nodiscard]] BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }
    [[nodiscard]] BufferPtr GetIndexBuffer() const { return m_indexBuffer; }
    [[nodiscard]] MaterialPtr GetMaterial() const { return m_material; }

    void SetMaterial(MaterialPtr material) { m_material = material; }

    void Draw(const Program* program) const;

private:
    Mesh()
    {
    }

    void Init(const eastl::vector<Vertex>& vertices, const eastl::vector<uint32_t>& indices, uint32_t primitiveType);

    uint32_t m_primitiveType { GL_TRIANGLES };

    // VertexLayout은 unique pointer를, Buffer는 shared pointer를 사용하는 이유
    // - VBO, IBO는 다른 VAO와 연결하여 재사용할 수 있다
    // - 반면 VAO는 해당 메쉬를 그리는데만 사용하게 될 것이다
    VertexLayoutUPtr m_vertexLayout;
    BufferPtr m_vertexBuffer;
    BufferPtr m_indexBuffer;
    MaterialPtr m_material;
};

#endif //__MESH_H__
