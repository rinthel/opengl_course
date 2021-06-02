#include "context.h"
#include "image.h"
#include <imgui.h>

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::ProcessInput(GLFWwindow* window) {
    if (!m_cameraControl)
        return;
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;    

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y) {
    if (!m_cameraControl)
        return;
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.8f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    m_prevMousePos = pos;    
}

void Context::MouseButton(int button, int action, double x, double y) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE) {
            m_cameraControl = false;
        }
    }
}

void Context::Render() {
    if (ImGui::Begin("ui window")) {
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();
        if (ImGui::Button("reset camera")) {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }
    }
    ImGui::End();

    m_cameraFront =
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f),
            glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto projection = glm::perspective(glm::radians(45.0f),
        (float)m_width / (float)m_height, 0.01f, 150.0f);
    auto view = glm::lookAt(
        m_cameraPos,
        m_cameraPos + m_cameraFront,
        m_cameraUp);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_pbrProgram->Use();
    m_pbrProgram->SetUniform("viewPos", m_cameraPos);
    m_pbrProgram->SetUniform("material.albedo", m_material.albedo);
    m_pbrProgram->SetUniform("material.ao", m_material.ao);
    for (size_t i = 0; i < m_lights.size(); i++) {
        auto posName = fmt::format("lights[{}].position", i);
        auto colorName = fmt::format("lights[{}].color", i);
        m_pbrProgram->SetUniform(posName, m_lights[i].position);
        m_pbrProgram->SetUniform(colorName, m_lights[i].color);
    }
    DrawScene(view, projection, m_pbrProgram.get());
}

bool Context::Init() {
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    m_box = Mesh::CreateBox();
    m_plane = Mesh::CreatePlane();
    m_sphere = Mesh::CreateSphere();

    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    m_pbrProgram = Program::Create("./shader/pbr.vs", "./shader/pbr.fs");

    m_lights.push_back({
        glm::vec3(5.0f, 5.0f, 6.0f), glm::vec3(20.0f, 20.0f, 20.0f)
    });
    m_lights.push_back({
        glm::vec3(-4.0f, 5.0f, 7.0f), glm::vec3(20.0f, 20.0f, 20.0f)
    });
    m_lights.push_back({
        glm::vec3(-4.0f, -6.0f, 8.0f), glm::vec3(20.0f, 20.0f, 20.0f)
    });
    m_lights.push_back({
        glm::vec3(5.0f, -6.0f, 9.0f), glm::vec3(20.0f, 20.0f, 20.0f)
    });
    return true;
}

void Context::DrawScene(const glm::mat4& view,
    const glm::mat4& projection,
    const Program* program) {

    program->Use();

    const int sphereCount = 7;
    const float offset = 1.2f;
    for (int j = 0; j < sphereCount; j++) {
        float y = ((float)j - (float)(sphereCount - 1) * 0.5f) * offset;
        for (int i = 0; i < sphereCount; i++) {
            float x = ((float)i - (float)(sphereCount - 1) * 0.5f) * offset;
            auto modelTransform =
                glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
            auto transform = projection * view * modelTransform;
            program->SetUniform("transform", transform);
            program->SetUniform("modelTransform", modelTransform);
            program->SetUniform("material.roughness", (float)i / (float)(sphereCount - 1));
            program->SetUniform("material.metallic", (float)j / (float)(sphereCount - 1));
            m_sphere->Draw(program);
        }
    }
}
