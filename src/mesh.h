#ifndef __MESH_H__
#define __MESH_H__

#include <EASTL/vector.h>

#include "common.h"
#include "buffer.h"
#include "vertex_layout.h"


struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};


CLASS_PTR(Mesh);


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

    void Draw() const;

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
};

#endif //__MESH_H__
