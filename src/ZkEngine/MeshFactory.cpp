#include "MeshFactory.hpp"
#include "glm/ext/scalar_constants.hpp"

namespace MeshFactory {

std::shared_ptr<Mesh> createLine() {
    std::vector<Vertex> vertices = {
        // position              normal           uv
        {{-5.0f, -5.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // 0
        {{5.0f, -5.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},   // 1
    };

    std::vector<uint32_t> indices = {0, 1};

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> createTriangle() {
    std::vector<Vertex> vertices = {
        // position              normal           uv
        {{-5.0f, -5.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // 0
        {{5.0f, -5.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},   // 1
        {{0.0f, 5.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.5f, 1.0f}},    // 2
    };

    std::vector<uint32_t> indices = {
        0, 1, 2, // Front
        2,1,0 // Back 
    };

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> createQuad() {
    std::vector<Vertex> vertices = {
        // position              normal           uv
        {{-5.0f, -5.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // 0
        {{5.0f, -5.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},   // 1
        {{5.0f, 5.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},    // 2
        {{-5.0f, 5.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},   // 3
    };

    std::vector<uint32_t> indices = {
        0, 1, 2,  //
        2, 3, 0   //
    };

    return std::make_shared<Mesh>(vertices, indices);
}

// Creates a cube with Vertex data [position, normal, uv]
std::shared_ptr<Mesh> createCube() {
    // This cube will work for lighting
    std::vector<Vertex> vertices = {
        // FRONT (+Z)
        {{-5, -5, 5}, {0, 0, 1}, {0, 0}},
        {{5, -5, 5}, {0, 0, 1}, {1, 0}},
        {{5, 5, 5}, {0, 0, 1}, {1, 1}},
        {{-5, 5, 5}, {0, 0, 1}, {0, 1}},

        // BACK (-Z)
        {{5, -5, -5}, {0, 0, -1}, {0, 0}},
        {{-5, -5, -5}, {0, 0, -1}, {1, 0}},
        {{-5, 5, -5}, {0, 0, -1}, {1, 1}},
        {{5, 5, -5}, {0, 0, -1}, {0, 1}},

        // LEFT (-X)
        {{-5, -5, -5}, {-1, 0, 0}, {0, 0}},
        {{-5, -5, 5}, {-1, 0, 0}, {1, 0}},
        {{-5, 5, 5}, {-1, 0, 0}, {1, 1}},
        {{-5, 5, -5}, {-1, 0, 0}, {0, 1}},

        // RIGHT (+X)
        {{5, -5, 5}, {1, 0, 0}, {0, 0}},
        {{5, -5, -5}, {1, 0, 0}, {1, 0}},
        {{5, 5, -5}, {1, 0, 0}, {1, 1}},
        {{5, 5, 5}, {1, 0, 0}, {0, 1}},

        // TOP (+Y)
        {{-5, 5, 5}, {0, 1, 0}, {0, 0}},
        {{5, 5, 5}, {0, 1, 0}, {1, 0}},
        {{5, 5, -5}, {0, 1, 0}, {1, 1}},
        {{-5, 5, -5}, {0, 1, 0}, {0, 1}},

        // BOTTOM (-Y)
        {{-5, -5, -5}, {0, -1, 0}, {0, 0}},
        {{5, -5, -5}, {0, -1, 0}, {1, 0}},
        {{5, -5, 5}, {0, -1, 0}, {1, 1}},
        {{-5, -5, 5}, {0, -1, 0}, {0, 1}},
    };

    std::vector<uint32_t> indices = {// Front
                                     0, 1, 2, 2, 3, 0,

                                     // Back
                                     4, 5, 6, 6, 7, 4,

                                     // Left
                                     8, 9, 10, 10, 11, 8,

                                     // Right
                                     12, 13, 14, 14, 15, 12,

                                     // Top
                                     16, 17, 18, 18, 19, 16,

                                     // Bottom
                                     20, 21, 22, 22, 23, 20};

    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> createSphere(float radius, int sectorCount, int stackCount) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float sectorStep = 2.0f * glm::pi<float>() / sectorCount;
    float stackStep = glm::pi<float>() / stackCount;

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = glm::pi<float>() / 2.0f - i * stackStep;  // +pi/2 to -pi/2
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

    return std::make_shared<Mesh>(vertices, indices);
}

}  // namespace MeshFactory
