#pragma once

#include "ZkEngine/Renderer/Vertex.hpp"

namespace zk {

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    MeshData(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
        : vertices(std::move(vertices)), indices(std::move(indices)) {}
    MeshData() = default;
};

namespace MeshFactory {

MeshData createLine();
MeshData createTriangle();
MeshData createQuad();
MeshData createCube();
MeshData createSphere(float radius = 0.5f, int sectorCount = 64, int stackCount = 64);

MeshData createGrid(int halfSize, float spacing = 1.0f);

MeshData createCameraBody();


} // namespace MeshFactory

} // namespace zk
