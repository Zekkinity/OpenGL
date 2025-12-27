#include "Renderer.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

#include "Texture.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "VertexBufferLayout.hpp"
#include "Shader.hpp"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

struct Object {
    glm::vec3 position = glm::vec3(200.0f, 200.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    float angle = 0.0f;

    glm::mat4 model = glm::mat4(1.0f);

    void update() {
        model = glm::mat4(1.0f);

        // Correct order is translation -> rotation -> scale
        model = glm::translate(model, position);

        // If you rotate after translation, it will rotate around the center of the object
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, scale);
    }
};

struct Camera2D {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    float zoom = 1.0f;
    float angle = 0.0f;
    glm::mat4 view = glm::mat4(1.0f);

    void update() {
        view = glm::mat4(1.0f);

        view = glm::rotate(view, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::translate(view, -position);
        view = glm::scale(view, glm::vec3(zoom, zoom, 1));
    }
};

int main() {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // This setups OpenGL 3.3 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    int width = 1920;
    int height = 1080;
    window = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        // Handle initialization error
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[] = {
        // x, y | texture coords
        -50.0f, -50.0f, 0.0f, 0.0f,  // 0
        50.0f,  -50.0f, 1.0f, 0.0f,  // 1
        50.0f,  50.0f,  1.0f, 1.0f,  // 2
        -50.0f, 50.0f,  0.0f, 1.0f,  // 3
    };

    unsigned int indices[] = {
        0, 1, 2,  // 0
        2, 3, 0,  // 1
    };

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    VertexArray va;
    VertexBuffer vb(positions, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    layout.push<float>(2);
    layout.push<float>(2);
    va.addBuffer(vb, layout);

    IndexBuffer ib(indices, 6);

    glm::mat4 proj = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
    // glm::mat4 proj =
    //     glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);

    Shader shader("../res/shaders/Basic.shader");
    shader.bind();

    shader.setUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    Texture texture("../res/textures/texture.png");
    // Texture texture("../res/textures/imagekev.jpg");
    texture.bind();
    shader.setUniform1i("u_Texture", 0);

    va.unbind();
    vb.unbind();
    ib.unbind();
    shader.unbind();

    Renderer renderer;

    ImGui::CreateContext();
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui::StyleColorsDark();

    float r = 0.0f;
    float increment = 0.05f;

    glm::mat4 mvp;

    Camera2D camera;

    Object object;
    Object object2;

    object2.position = glm::vec3(400.0f, 400.0f, 0.0f);

    while (!glfwWindowShouldClose(window)) {
        renderer.clear();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        camera.update();

        shader.bind();
        // shader.setUniform4f("u_Color", r, 0.0f, 1.0f, 1.0f);

        {
            object.update();
            mvp = proj * camera.view * object.model;
            shader.setUniformMat4f("u_MVP", mvp);
            renderer.draw(va, ib, shader);
        }

        {
            object2.update();
            mvp = proj * camera.view * object2.model;

            shader.setUniformMat4f("u_MVP", mvp);
            renderer.draw(va, ib, shader);
        }

        if (r > 1.0f)
            increment = -0.05f;
        else if (r < 0.0f)
            increment = 0.05f;

        r += increment;

        {
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("object");
            ImGui::SliderFloat3("Translation", &object.position.x, 0.0f, width);
            ImGui::SliderFloat2("Scale", &object.scale.x, 1.0f, 10.0f);
            ImGui::SliderFloat("Rotation", &object.angle, 0.0f, 360.0f);
            ImGui::Text("object2");
            ImGui::SliderFloat3("Translation2", &object2.position.x, 0.0f, width);
            ImGui::SliderFloat2("Scale2", &object2.scale.x, 1.0f, 10.0f);
            ImGui::SliderFloat("Rotation2", &object2.angle, 0.0f, 360.0f);

            ImGui::Text("camera");
            ImGui::SliderFloat3("pos", &camera.position.x, -1000.0f, 1000.0f);
            ImGui::SliderFloat("angle", &camera.angle, 0.0f, 360.0f);
            ImGui::SliderFloat("zoom", &camera.zoom, 0.1f, 10.0f);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
