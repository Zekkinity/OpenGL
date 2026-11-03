#include "zkpch.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLDevice.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLVertexBuffer.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLIndexBuffer.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLMesh.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLShader.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLTexture2D.hpp"
#include "ZkEngine/Renderer/API/OpenGL/OpenGLTextureCube.hpp"

#include "ZkEngine/Renderer/API/OpenGL/OpenGLFramebuffer.hpp"

#include "ZkEngine/Renderer/Model.hpp"

#include "ZkEngine/Renderer/BatchRenderer.hpp"

#include "ZkEngine/Renderer/API/OpenGL/Debug/OpenGLDebug.hpp"

namespace zk {

void OpenGLDevice::init() {
    OpenGLDebug::init(true);
}

Scope<VertexBuffer> OpenGLDevice::createVertexBuffer(
    std::span<const std::byte> vertices, VertexBufferUsage usage) {
    return createScope<OpenGLVertexBuffer>(vertices, usage);
}

Scope<IndexBuffer> OpenGLDevice::createIndexBuffer(std::span<const uint32_t> indices) {
    return createScope<OpenGLIndexBuffer>(indices);
}

Ref<Mesh> OpenGLDevice::createMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices) {
    return createRef<OpenGLMesh>(*this, vertices, indices);
}

Ref<Shader> OpenGLDevice::createShader(
    std::string_view vertexPath, std::string_view fragmentPath, std::string_view geometryPath) {
    return createRef<OpenGLShader>(vertexPath, fragmentPath, geometryPath);
}

Ref<Texture> OpenGLDevice::loadTexture(
    const std::filesystem::path& path, const TextureSpecs& specs) {
    auto image = ImageLoader::load(path);

    TextureSpecs finalSpecs = specs;
    finalSpecs.width = image.width;
    finalSpecs.height = image.height;

    auto texture = createRef<OpenGLTexture2D>(finalSpecs, image);

    ImageLoader::free(image);

    return texture;
}

Ref<Texture> OpenGLDevice::createTexture(const TextureSpecs& specs, const ImageData& imageData) {
    return createRef<OpenGLTexture2D>(specs, imageData);
}

Ref<Texture> OpenGLDevice::loadTextureHDR(const std::string& path) {
    auto image = ImageLoader::load(path, false);

    TextureSpecs finalSpecs{
        .width = image.width,
        .height = image.height,
        .format = TextureFormat::RGBA16F,
        .filter = TextureFilter::Linear,
        .wrap = TextureWrap::ClampToEdge,
    };

    Ref<OpenGLTexture2D> texture = createRef<OpenGLTexture2D>(finalSpecs, image);

    ImageLoader::free(image);

    return texture;
}

Ref<Texture> OpenGLDevice::loadTextureCube(const std::vector<std::filesystem::path>& faces) {
    ZK_CORE_ASSERT(faces.size() == 6, "TextureCube must have 6 faces!");

    std::array<ImageData, 6> images;

    for (uint32_t i = 0; i < 6; i++) {
        images[i] = ImageLoader::load(faces[i], false);
        if (!images[i].data) {
            return nullptr;
        }
    }

    TextureSpecs specs;
    specs.width = images[0].width;
    specs.height = images[0].height;

    auto texture = createRef<OpenGLTextureCube>(specs, images);
    for (auto& img : images) {
        ImageLoader::free(img);
    }

    return texture;
}

Ref<Texture> OpenGLDevice::createTextureCube(const TextureSpecs& specs) {
    return createRef<OpenGLTextureCube>(specs);
}

Ref<Model> OpenGLDevice::loadModel(const std::filesystem::path& path) {
    return createRef<Model>(*this, path);
}

Scope<Framebuffer> OpenGLDevice::createFramebuffer(const FramebufferSpec& specs) {
    return createScope<OpenGLFramebuffer>(specs);
}

// DRAW CALLS
void OpenGLDevice::drawBatch(const Batch& batch) {
    int count = batch.key.mesh->getIndexBuffer().getCount();

    glDrawElementsInstancedBaseInstance(
        GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, batch.instanceCount, batch.instanceOffset);

    // m_Stats.drawCalls++;
    // uint32_t trianglesPerInstance = count / 3;
    // m_Stats.triangleCount += trianglesPerInstance * batch.instanceCount;
    // m_Stats.indexCount += count * batch.instanceCount;
}

void OpenGLDevice::drawMesh(const Ref<Mesh>& mesh) {
    if (!mesh) {
        ZK_CORE_ERROR("Invalid mesh");
        return;
    }

    uint32_t indexCount = mesh->getIndexBuffer().getCount();

    if (indexCount <= 0) {
        ZK_CORE_ERROR("Invalid index count: {}", indexCount);
        return;
    }

    mesh->bind();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    // glMultiDrawElementsIndirect()
    // glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, 20);

    // m_Stats.drawCalls++;
    // m_Stats.triangleCount += indexCount / 3;
    // m_Stats.indexCount += indexCount;

    // ZK_CORE_DEBUG("CALLED MESH");
}

void OpenGLDevice::drawScreenQuad() {
    if (m_QuadVAO == 0) {
        float quadVertices[] = {                          // positions   // texCoords
                                -1.0f, 1.0f,  0.0f, 1.0f, //
                                -1.0f, -1.0f, 0.0f, 0.0f, //
                                1.0f,  -1.0f, 1.0f, 0.0f, //

                                -1.0f, 1.0f,  0.0f, 1.0f, //
                                1.0f,  -1.0f, 1.0f, 0.0f, //
                                1.0f,  1.0f,  1.0f, 1.0f};

        glGenVertexArrays(1, &m_QuadVAO);
        glGenBuffers(1, &m_QuadVBO);

        glBindVertexArray(m_QuadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

} // namespace zk
