#include "ElectronWorld.hpp"

#include "ZkEngine/Core/AppAcess.hpp"
#include "imgui.h"

#include <print>

void ElectronWorld::init() {
    m_Particles.reserve(100);

    glm::vec3 p1Position = glm::vec3(30, 0.0, 0);
    // glm::vec3 p1Velocity = glm::vec3(0, 0, 1);
    glm::vec3 p1Velocity = glm::vec3(0);
    float p1Charge = +1.0f;
    addParticle(p1Position, p1Velocity, p1Charge);

    glm::vec3 p2Position = glm::vec3(-30, 0.0, 0);
    glm::vec3 p2Velocity = glm::vec3(0);
    float p2Charge = -1.0f;
    addParticle(p2Position, p2Velocity, p2Charge);

    // m_Point = std::make_unique<zk::RenderObject>(m_SphereMesh, m_ParticleMaterial);
    // m_Point->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);


    m_SphereMesh = GetAssets().getMesh(zk::PrimitiveType::Sphere);
    auto grid = GetAssets().getMesh(zk::PrimitiveType::Grid);
    // auto grid = zk::Renderer::createGrid(50);
    // m_Grid = std::make_unique<zk::RenderObject>(grid, m_ParticleMaterial);
}

// void ElectronWorld::update(float dt) {
//     manageElectrons(dt);
//
//     m_Lines.clear();
//     allLines.clear();
//
//     for (auto& p : m_Particles) {
//
//         float radius = 5.0f;
//
//         int latSteps = sqrt(m_SeedCount); // vertical rings
//         int lonSteps = sqrt(m_SeedCount); // around rings
//
//         for (int i = 0; i < latSteps; i++) {
//             float phi = glm::pi<float>() * (i + 0.5f) / latSteps;
//
//             for (int j = 0; j < lonSteps; j++) {
//                 float theta = glm::two_pi<float>() * j / lonSteps;
//
//                 glm::vec3 dir(
//                     sin(phi) * cos(theta),
//                     sin(phi) * sin(theta),
//                     cos(phi)
//                 );
//
//                 glm::vec3 seed = p->object->transform.position + dir * radius;
//
//                 std::vector<glm::vec3> line;
//                 glm::vec3 pos = seed;
//
//                 for (int step = 0; step < m_Steps; step++) {
//                     glm::vec3 E = getElectricFieldAtPoint(pos);
//
//                     if (glm::length(E) < 0.01f)
//                         break;
//
//                     line.push_back(pos);
//                     pos += glm::normalize(E) * 1.0f;
//                 }
//
//                 if (line.size() > 1)
//                     allLines.push_back(line);
//             }
//         }
//     }
//
//     // std::println("Field: {}, {}, {}", field.x, field.y, field.z);
// }

void ElectronWorld::update(float dt) {
    manageElectrons(dt);

    m_Lines.clear();
    allLines.clear();

    const float seedRadius = 5.0f;
    const float minStep = 0.1f;
    const float maxStep = 1.0f;
    const float stepScale = 2.0f; // controls adaptive stepping
    const float stopField = 0.01f;
    const float stopRadius = 0.5f;   // stop near charges
    const float worldLimit = 200.0f; // prevent runaway lines
    const int maxSteps = m_Steps;

    const float goldenAngle = glm::pi<float>() * (3.0f - sqrt(5.0f));

    for (auto& p : m_Particles) {
        // glm::vec3 chargePos = p->transform.position;
        glm::vec3 chargePos = p->entity.getPosition();

        // --- Fibonacci sphere sampling ---
        for (int i = 0; i < m_SeedCount; i++) {
            float y = 1.0f - (i / float(m_SeedCount - 1)) * 2.0f;
            float radius = sqrt(1.0f - y * y);
            float theta = goldenAngle * i;

            float x = cos(theta) * radius;
            float z = sin(theta) * radius;

            glm::vec3 dir(x, y, z);
            glm::vec3 seed = chargePos + dir * seedRadius;

            // Trace forward and backward
            std::vector<glm::vec3> forward =
                traceFieldLine(seed, +1.0f, minStep, maxStep, stepScale, stopField, stopRadius, worldLimit, maxSteps);

            std::vector<glm::vec3> backward =
                traceFieldLine(seed, -1.0f, minStep, maxStep, stepScale, stopField, stopRadius, worldLimit, maxSteps);

            // Merge backward (reverse) + forward
            if (!forward.empty() || !backward.empty()) {
                std::vector<glm::vec3> fullLine;

                std::reverse(backward.begin(), backward.end());
                fullLine.insert(fullLine.end(), backward.begin(), backward.end());
                fullLine.push_back(seed);
                fullLine.insert(fullLine.end(), forward.begin(), forward.end());

                if (fullLine.size() > 1)
                    allLines.push_back(fullLine);
            }
        }
    }
}

std::vector<glm::vec3> ElectronWorld::traceFieldLine(
    glm::vec3 start,
    float direction,
    float minStep,
    float maxStep,
    float stepScale,
    float stopField,
    float stopRadius,
    float worldLimit,
    int maxSteps) {
    std::vector<glm::vec3> line;
    glm::vec3 pos = start;

    for (int step = 0; step < maxSteps; step++) {
        glm::vec3 E = getElectricFieldAtPoint(pos);
        float strength = glm::length(E);

        // Stop if field too weak
        if (strength < stopField)
            break;

        // Stop if outside world
        if (glm::length(pos) > worldLimit)
            break;

        // Stop if too close to any charge
        for (auto& p : m_Particles) {
            // float dist = glm::distance(pos, p->transform.position);
            float dist = glm::distance(pos, p->entity.getPosition());
            if (dist < stopRadius)
                return line;
        }

        line.push_back(pos);

        // Adaptive step size
        float h = glm::clamp(stepScale / strength, minStep, maxStep);

        // --- RK4 integration ---
        glm::vec3 k1 = direction * glm::normalize(getElectricFieldAtPoint(pos));
        glm::vec3 k2 = direction * glm::normalize(getElectricFieldAtPoint(pos + 0.5f * h * k1));
        glm::vec3 k3 = direction * glm::normalize(getElectricFieldAtPoint(pos + 0.5f * h * k2));
        glm::vec3 k4 = direction * glm::normalize(getElectricFieldAtPoint(pos + h * k3));

        glm::vec3 delta = (h / 6.0f) * (k1 + 2.0f * k2 + 2.0f * k3 + k4);
        pos += delta;
    }

    return line;
}

// void ElectronWorld::submit(zk::RendererBackend& renderer) const {
//     // if (!m_Camera)
//     //     return;
//
//     // for (const auto& particle : m_Particles) {
//     //     renderer.submit(*particle->object);
//     // }
//     //
//     // renderer.submit(*m_Point);
//     // // renderer.submit(*m_Grid, *m_Camera);
//     //
//     // for (const auto& line : allLines) {
//     //     renderer.drawLine(line);
//     // }
// }

void ElectronWorld::debug() {
    for (auto& p : m_Particles) {
        ImGui::PushID(p.get());
        ImGui::SliderFloat("Charge", &p->charge, -20.0f, 20.0f);
        ImGui::PopID();
    }

    ImGui::SliderInt("Steps", &m_Steps, 1, 500);
    ImGui::SliderInt("Seed Count", &m_SeedCount, 1, 100);

    ImGui::Text("Particle Count: %zu", m_Particles.size());

    if (ImGui::Button("Randomize Charges")) {
        float charge = static_cast<float>(rand() / (RAND_MAX / 2.0f)) - 1.0f;
        m_Particles[0]->charge = charge;
        std::println("Charge: {}", charge);
    }

    if (ImGui::Button("Add Particle")) {
        glm::vec3 position = glm::vec3(20, 0.0, 20);
        glm::vec3 velocity = glm::vec3(0, 0, -1);
        // generate random number between -1 and 1
        float charge = static_cast<float>(rand() / (RAND_MAX / 2.0f)) - 1.0f;

        addParticle(position, velocity, charge);
    }
}

void ElectronWorld::manageElectrons(float dt) {
    // Reset forces
    for (auto& p : m_Particles) {
        p->force = glm::vec3(0.0f);
    }

    // calculate forces
    for (size_t i = 0; i < m_Particles.size(); i++) {
        for (size_t j = i + 1; j < m_Particles.size(); j++) {
            auto& p1 = m_Particles[i];
            auto& p2 = m_Particles[j];

            // glm::vec3 direction = p1->transform.position - p2->transform.position;
            glm::vec3 direction = p1->entity.getPosition() - p2->entity.getPosition();
            float r = glm::length(direction);

            // Avoid division by zero
            if (r < 1e-6f)
                continue;

            glm::vec3 dir = glm::normalize(direction);

            float forceMagnitude = K * (p1->charge * p2->charge) / (r * r);

            glm::vec3 forceVector = dir * forceMagnitude;

            // p1->velocity += forceVector / MASS * dt;
            // p2->velocity -= forceVector / MASS * dt;
            p1->force += forceVector;
            p2->force -= forceVector;
            // std::println("Force: {}, Distance: {}", forceVector.x, r);
        }
    }

    // update positions
    // for (auto& p : m_Particles) {
    //     p->velocity += p->force / MASS * dt;
    //     p->object->transform.position += p->velocity * dt;
    // }
}

void ElectronWorld::calculateElectricField() {
    // Get all points inside a cube
    glm::vec3 samplePoint = glm::vec3(0.0f);

    glm::vec3 totalField(0.0f);

    for (const auto& p : m_Particles) {
        // glm::vec3 rVec = point - p->object->transform.position;
        // glm::vec3 rVec = getCamera()->getPosition() - p->transform.position;
        // glm::vec3 rVec = getCamera()->getPosition() - p->entity.getPosition();
        // float r = glm::length(rVec);
        //
        // if (r < 1e-6f)
        //     continue;
        //
        // totalField += K * p->charge * rVec / (r * r * r);
    }

    std::println("Total Field: {}, {}, {}", totalField.x, totalField.y, totalField.z);
}

glm::vec3 ElectronWorld::getElectricFieldAtPoint(const glm::vec3& position) {
    glm::vec3 totalField(0.0f);

    for (const auto& p : m_Particles) {
        // glm::vec3 rVec = position - p->transform.position;
        glm::vec3 rVec = position - p->entity.getPosition();
        float r = glm::length(rVec);

        if (r < 1e-6f)
            continue;

        totalField += K * p->charge * rVec / (r * r * r);
    }

    return totalField;
}

void ElectronWorld::addParticle(const glm::vec3& position, const glm::vec3& velocity, float charge) {
    // newParticle->object = newParticle->entity.addComponent<zk::RenderObject>(m_SphereMesh, m_ParticleMaterial);
    //
    // newParticle->charge = charge;
    // // if (newParticle->charge > 0.0f)
    // // newParticle->object->setColor({1.0f, 0.0f, 0.0f, 1.0f});
    // // else
    // // newParticle->object->setColor({0.0f, 0.0f, 1.0f, 1.0f});
    //
    // newParticle->velocity = velocity;
    // newParticle->transform.position = position;
    // m_Particles.push_back(std::move(newParticle));
}
