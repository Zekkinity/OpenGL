#pragma once

#include <memory>
#include "Mesh.hpp"

namespace zk::MeshFactory {

std::shared_ptr<Mesh> createLine();
std::shared_ptr<Mesh> createTriangle();
std::shared_ptr<Mesh> createQuad();
std::shared_ptr<Mesh> createCube();
std::shared_ptr<Mesh> createSphere(float radius = 0.5f, int sectorCount = 64, int stackCount = 64);

std::shared_ptr<Mesh> createGrid(int halfSize, float spacing = 1.0f);

}  // namespace MeshFactory
