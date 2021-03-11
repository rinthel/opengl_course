#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"

CLASS_PTR(Context)
class Context {
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;
    ProgramUPtr m_simpleProgram;

    MeshUPtr m_box;
    ModelUPtr m_model;

    int m_width {640};
    int m_height {480};

    // animation
    bool m_animation { true };

    // clear color
    glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };

    // light parameter
    struct Light {
        glm::vec3 position { glm::vec3(2.0f, 2.0f, 2.0f) };
        glm::vec3 direction { glm::vec3(-1.0f, -1.0f, -1.0f) };
        glm::vec2 cutoff { glm::vec2(20.0f, 5.0f) };
        float distance { 32.0f };
        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.8f, 0.8f, 0.8f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };
    Light m_light;

    // material parameter
    MaterialPtr m_material;

    // camera parameter
    bool m_cameraControl { false };
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    float m_cameraPitch { 0.0f };
    float m_cameraYaw { 0.0f };
    glm::vec3 m_cameraFront { glm::vec3(0.0f, -1.0f, 0.0f) };
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f, 3.0f) };
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };
};

#endif // __CONTEXT_H__