#include "Renderer.hpp"

#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR)
        ;
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line
                  << '\n';
        return false;
    }
    return true;
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
    // Binds the shader to render whatever is in the vertex array
    shader.bind();

    // Binds the vertex array, this contains all the vertex data
    va.bind();

    // Binds the index buffer, this contains the indices into the vertex buffer
    ib.bind();

    GLCall(glDrawElements(GL_TRIANGLES, ib.getCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::clear() const {
    glClear(GL_COLOR_BUFFER_BIT);

    // 3D
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
