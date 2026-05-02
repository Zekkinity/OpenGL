#include "zkpch.hpp"
#include "MeshFactory.hpp"

#include "ZkEngine/Renderer/Vertex.hpp"
#include "glm/ext/scalar_constants.hpp"

namespace zk::MeshFactory {

MeshData createLine() {
    std::vector<Vertex> vertices = {
        // position              normal           uv
        {{-0.5, -0.5, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // 0
        {{0.5, -0.5, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 1
    };

    std::vector<uint32_t> indices = {0, 1};

    return {std::move(vertices), std::move(indices)};
}

MeshData createTriangle() {
    std::vector<Vertex> vertices = {
        // position              normal           uv
        {{-0.5, -0.5, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // 0
        {{0.5, -0.5, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 1
        {{0.0f, 0.5, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}},  // 2
    };

    std::vector<uint32_t> indices = {
        0,
        1,
        2, // Front
        2,
        1,
        0 // Back
    };

    return {std::move(vertices), std::move(indices)};
}

MeshData createQuad() {
    std::vector<Vertex> vertices = {
        // position              normal           uv
        {{-0.5, -0.5, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // 0
        {{0.5, -0.5, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // 1
        {{0.5, 0.5, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // 2
        {{-0.5, 0.5, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // 3
    };

    std::vector<uint32_t> indices = {
        0,
        1,
        2, //
        2,
        3,
        0 //
    };

    return {std::move(vertices), std::move(indices)};
}

// Creates a cube with Vertex data [position, normal, uv]
MeshData createCube() {
    // This cube will work for lighting
    std::vector<Vertex> vertices = {
        // FRONT (+Z)
        {{-0.5, -0.5, 0.5}, {0, 0, 1}, {0, 0}},
        {{0.5, -0.5, 0.5}, {0, 0, 1}, {1, 0}},
        {{0.5, 0.5, 0.5}, {0, 0, 1}, {1, 1}},
        {{-0.5, 0.5, 0.5}, {0, 0, 1}, {0, 1}},

        // BACK (-Z)
        {{0.5, -0.5, -0.5}, {0, 0, -1}, {0, 0}},
        {{-0.5, -0.5, -0.5}, {0, 0, -1}, {1, 0}},
        {{-0.5, 0.5, -0.5}, {0, 0, -1}, {1, 1}},
        {{0.5, 0.5, -0.5}, {0, 0, -1}, {0, 1}},

        // LEFT (-X)
        {{-0.5, -0.5, -0.5}, {-1, 0, 0}, {0, 0}},
        {{-0.5, -0.5, 0.5}, {-1, 0, 0}, {1, 0}},
        {{-0.5, 0.5, 0.5}, {-1, 0, 0}, {1, 1}},
        {{-0.5, 0.5, -0.5}, {-1, 0, 0}, {0, 1}},

        // RIGHT (+X)
        {{0.5, -0.5, 0.5}, {1, 0, 0}, {0, 0}},
        {{0.5, -0.5, -0.5}, {1, 0, 0}, {1, 0}},
        {{0.5, 0.5, -0.5}, {1, 0, 0}, {1, 1}},
        {{0.5, 0.5, 0.5}, {1, 0, 0}, {0, 1}},

        // TOP (+Y)
        {{-0.5, 0.5, 0.5}, {0, 1, 0}, {0, 0}},
        {{0.5, 0.5, 0.5}, {0, 1, 0}, {1, 0}},
        {{0.5, 0.5, -0.5}, {0, 1, 0}, {1, 1}},
        {{-0.5, 0.5, -0.5}, {0, 1, 0}, {0, 1}},

        // BOTTOM (-Y)
        {{-0.5, -0.5, -0.5}, {0, -1, 0}, {0, 0}},
        {{0.5, -0.5, -0.5}, {0, -1, 0}, {1, 0}},
        {{0.5, -0.5, 0.5}, {0, -1, 0}, {1, 1}},
        {{-0.5, -0.5, 0.5}, {0, -1, 0}, {0, 1}},
    };

    std::vector<uint32_t> indices = {
        // Front
        0,
        1,
        2,
        2,
        3,
        0,

        // Back
        4,
        5,
        6,
        6,
        7,
        4,

        // Left
        8,
        9,
        10,
        10,
        11,
        8,

        // Right
        12,
        13,
        14,
        14,
        15,
        12,

        // Top
        16,
        17,
        18,
        18,
        19,
        16,

        // Bottom
        20,
        21,
        22,
        22,
        23,
        20};

    return {std::move(vertices), std::move(indices)};
}

MeshData createSphere(float radius, int sectorCount, int stackCount) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float sectorStep = 2.0f * glm::pi<float>() / sectorCount;
    float stackStep = glm::pi<float>() / stackCount;

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = glm::pi<float>() / 2.0f - i * stackStep; // +pi/2 to -pi/2
        float xy = radius * cos(stackAngle);
        float y = radius * sin(stackAngle);

        float v = static_cast<float>(i) / stackCount;

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * sectorStep;

            float x = xy * cos(sectorAngle);
            float z = xy * sin(sectorAngle);

            Vertex vertex;
            vertex.position = glm::vec3(x, y, z);
            vertex.normal = glm::normalize(vertex.position);

            float u = static_cast<float>(j) / sectorCount;
            vertex.texCoords = glm::vec2(u, v);

            vertices.push_back(vertex);
        }
    }

    for (int i = 0; i < stackCount; ++i) {
        uint32_t k1 = i * (sectorCount + 1);
        uint32_t k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k1 + 1);
                indices.push_back(k2);
            }

            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2 + 1);
                indices.push_back(k2);
            }
        }
    }
    return {std::move(vertices), std::move(indices)};
}

MeshData createGrid(int halfSize, float spacing) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    uint32_t index = 0;

    for (int i = -halfSize; i <= halfSize; i++) {
        float p = i * spacing;

        // Line parallel to X (along Z)
        vertices.push_back({{-halfSize * spacing, 0.0f, p}, {0, 1, 0}, {}});
        vertices.push_back({{halfSize * spacing, 0.0f, p}, {0, 1, 0}, {}});

        indices.push_back(index++);
        indices.push_back(index++);

        // Line parallel to Z (along X)
        vertices.push_back({{p, 0.0f, -halfSize * spacing}, {0, 1, 0}, {}});
        vertices.push_back({{p, 0.0f, halfSize * spacing}, {0, 1, 0}, {}});

        indices.push_back(index++);
        indices.push_back(index++);
    }

    return {std::move(vertices), std::move(indices)};
}

MeshData createCameraBody() {
    std::vector<Vertex> vertices = {
        // Body - 8 corners of main box
        // front face
        {{-0.6f, -0.4f, 0.4f}}, // 0 front bottom left
        {{0.6f, -0.4f, 0.4f}},  // 1 front bottom right
        {{0.6f, 0.4f, 0.4f}},   // 2 front top right
        {{-0.6f, 0.4f, 0.4f}},  // 3 front top left
        // back face
        {{-0.6f, -0.4f, -0.4f}}, // 4 back bottom left
        {{0.6f, -0.4f, -0.4f}},  // 5 back bottom right
        {{0.6f, 0.4f, -0.4f}},   // 6 back top right
        {{-0.6f, 0.4f, -0.4f}},  // 7 back top left

        // Lens - smaller box protruding from front
        {{-0.2f, -0.2f, 0.4f}}, // 8  lens back bottom left
        {{0.2f, -0.2f, 0.4f}},  // 9  lens back bottom right
        {{0.2f, 0.2f, 0.4f}},   // 10 lens back top right
        {{-0.2f, 0.2f, 0.4f}},  // 11 lens back top left
        {{-0.2f, -0.2f, 0.7f}}, // 12 lens front bottom left
        {{0.2f, -0.2f, 0.7f}},  // 13 lens front bottom right
        {{0.2f, 0.2f, 0.7f}},   // 14 lens front top right
        {{-0.2f, 0.2f, 0.7f}},  // 15 lens front top left

        // Hotshoe on top - flat box
        {{-0.15f, 0.4f, 0.1f}},   // 16
        {{0.15f, 0.4f, 0.1f}},    // 17
        {{0.15f, 0.55f, 0.1f}},   // 18
        {{-0.15f, 0.55f, 0.1f}},  // 19
        {{-0.15f, 0.4f, -0.1f}},  // 20
        {{0.15f, 0.4f, -0.1f}},   // 21
        {{0.15f, 0.55f, -0.1f}},  // 22
        {{-0.15f, 0.55f, -0.1f}}, // 23
    };

    std::vector<uint32_t> indices = {
        // Body front face
        0,
        1,
        1,
        2,
        2,
        3,
        3,
        0,
        // Body back face
        4,
        5,
        5,
        6,
        6,
        7,
        7,
        4,
        // Body connecting edges
        0,
        4,
        1,
        5,
        2,
        6,
        3,
        7,

        // Lens back face
        8,
        9,
        9,
        10,
        10,
        11,
        11,
        8,
        // Lens front face
        12,
        13,
        13,
        14,
        14,
        15,
        15,
        12,
        // Lens connecting edges
        8,
        12,
        9,
        13,
        10,
        14,
        11,
        15,

        // Hotshoe
        16,
        17,
        17,
        18,
        18,
        19,
        19,
        16,
        20,
        21,
        21,
        22,
        22,
        23,
        23,
        20,
        16,
        20,
        17,
        21,
        18,
        22,
        19,
        23,
    };

    MeshData data;
    data.vertices = vertices;
    data.indices = indices;
    return data;
}

} // namespace zk::MeshFactory
