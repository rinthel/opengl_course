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
        if (ImGui::CollapsingHeader("lights")) {
            static int lightIndex = 0;
            ImGui::DragInt("light.index", &lightIndex, 1.0f, 0, (int)m_lights.size() - 1);
            ImGui::DragFloat3("light.pos", glm::value_ptr(m_lights[lightIndex].position), 0.01f);
            ImGui::DragFloat3("light.color", glm::value_ptr(m_lights[lightIndex].color), 0.1f);
        }
        if (ImGui::CollapsingHeader("material")) {
            ImGui::ColorEdit3("mat.albedo", glm::value_ptr(m_material.albedo));
            ImGui::SliderFloat("mat.roughness", &m_material.roughness, 0.0f, 1.0f);
            ImGui::SliderFloat("mat.metallic", &m_material.metallic, 0.0f, 1.0f);
            ImGui::SliderFloat("mat.ao", &m_material.ao, 0.0f, 1.0f);
        }
        ImGui::Checkbox("use IBL", &m_useIBL);

        float w = ImGui::GetContentRegionAvailWidth();
        ImGui::Image((ImTextureID)m_brdfLookupMap->Get(), ImVec2(w, w));
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
    m_pbrProgram->SetUniform("material.ao", m_material.ao);
    m_pbrProgram->SetUniform("material.albedo", m_material.albedo);
    m_pbrProgram->SetUniform("useIBL", m_useIBL ? 1 : 0);
    m_pbrProgram->SetUniform("irradianceMap", 0);
    m_pbrProgram->SetUniform("preFilteredMap", 1);
    m_pbrProgram->SetUniform("brdfLookupTable", 2);
    glActiveTexture(GL_TEXTURE0);
    m_diffuseIrradianceMap->Bind();
    glActiveTexture(GL_TEXTURE1);
    m_preFilteredMap->Bind();
    glActiveTexture(GL_TEXTURE2);
    m_brdfLookupMap->Bind();
    glActiveTexture(GL_TEXTURE0);
    for (size_t i = 0; i < m_lights.size(); i++) {
        auto posName = fmt::format("lights[{}].position", i);
        auto colorName = fmt::format("lights[{}].color", i);
        m_pbrProgram->SetUniform(posName, m_lights[i].position);
        m_pbrProgram->SetUniform(colorName, m_lights[i].color);
    }
    DrawScene(view, projection, m_pbrProgram.get());

    glDepthFunc(GL_LEQUAL);
    m_skyboxProgram->Use();
    m_skyboxProgram->SetUniform("projection", projection);
    m_skyboxProgram->SetUniform("view", view);
    m_skyboxProgram->SetUniform("cubeMap", 0);
    m_hdrCubeMap->Bind();
    m_box->Draw(m_skyboxProgram.get());
    glDepthFunc(GL_LESS);
}

bool Context::Init() {
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    m_box = Mesh::CreateBox();
    m_plane = Mesh::CreatePlane();
    m_sphere = Mesh::CreateSphere();

    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    m_pbrProgram = Program::Create("./shader/pbr.vs", "./shader/pbr.fs");

    m_lights.push_back({
        glm::vec3(5.0f, 5.0f, 6.0f), glm::vec3(40.0f, 40.0f, 40.0f)
    });
    m_lights.push_back({
        glm::vec3(-4.0f, 5.0f, 7.0f), glm::vec3(40.0f, 40.0f, 40.0f)
    });
    m_lights.push_back({
        glm::vec3(-4.0f, -6.0f, 8.0f), glm::vec3(40.0f, 40.0f, 40.0f)
    });
    m_lights.push_back({
        glm::vec3(5.0f, -6.0f, 9.0f), glm::vec3(40.0f, 40.0f, 40.0f)
    });

    m_hdrMap = Texture::CreateFromImage(
        Image::Load("./image/Alexs_Apt_2k.hdr").get());
    m_sphericalMapProgram = Program::Create(
        "./shader/spherical_map.vs", "./shader/spherical_map.fs");

    m_hdrCubeMap = CubeTexture::Create(512, 512, GL_RGB16F, GL_FLOAT);
    auto cubeFramebuffer = CubeFramebuffer::Create(m_hdrCubeMap);
    auto projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    std::vector<glm::mat4> views = {
        glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    };
    m_sphericalMapProgram->Use();
    m_sphericalMapProgram->SetUniform("tex", 0);
    m_hdrMap->Bind();
    glViewport(0, 0, 512, 512);
    for (int i = 0; i < (int)views.size(); i++) {
        cubeFramebuffer->Bind(i);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_sphericalMapProgram->SetUniform("transform", projection * views[i]);
        m_box->Draw(m_sphericalMapProgram.get());
    }
    m_hdrCubeMap->GenerateMipmap();

    m_diffuseIrradianceProgram = Program::Create(
        "./shader/skybox_hdr.vs", "./shader/diffuse_irradiance.fs");
    m_diffuseIrradianceMap = CubeTexture::Create(64, 64, GL_RGB16F, GL_FLOAT);
    cubeFramebuffer = CubeFramebuffer::Create(m_diffuseIrradianceMap);
    glDepthFunc(GL_LEQUAL);
    m_diffuseIrradianceProgram->Use();
    m_diffuseIrradianceProgram->SetUniform("projection", projection);
    m_diffuseIrradianceProgram->SetUniform("cubeMap", 0);
    m_hdrCubeMap->Bind();
    glViewport(0, 0, 64, 64);
    for (int i = 0; i < (int)views.size(); i++) {
        cubeFramebuffer->Bind(i);
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_diffuseIrradianceProgram->SetUniform("view", views[i]);
        m_box->Draw(m_diffuseIrradianceProgram.get());
    }
    glDepthFunc(GL_LESS);

    const uint32_t maxMipLevels = 5;
    glDepthFunc(GL_LEQUAL);
    m_preFilteredProgram = Program::Create(
        "./shader/skybox_hdr.vs", "./shader/prefiltered_light.fs");
    m_preFilteredMap = CubeTexture::Create(128, 128, GL_RGB16F, GL_FLOAT);
    m_preFilteredMap->GenerateMipmap();
    m_preFilteredProgram->Use();
    m_preFilteredProgram->SetUniform("projection", projection);
    m_preFilteredProgram->SetUniform("cubeMap", 0);
    m_hdrCubeMap->Bind();
    for (uint32_t mip = 0; mip < maxMipLevels; mip++) {
        auto framebuffer = CubeFramebuffer::Create(m_preFilteredMap, mip);
        uint32_t mipWidth = 128 >> mip;
        uint32_t mipHeight = 128 >> mip;
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        m_preFilteredProgram->SetUniform("roughness", roughness);
        for (uint32_t i = 0; i < (int)views.size(); i++) {
            m_preFilteredProgram->SetUniform("view", views[i]);
            framebuffer->Bind(i);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_box->Draw(m_preFilteredProgram.get());   
        }
    }
    glDepthFunc(GL_LESS);

    m_brdfLookupProgram = Program::Create(
        "./shader/brdf_lookup.vs", "./shader/brdf_lookup.fs");
    m_brdfLookupMap = Texture::Create(512, 512, GL_RG16F, GL_FLOAT);
    auto lookupFramebuffer = Framebuffer::Create({ m_brdfLookupMap });
    lookupFramebuffer->Bind();
    glViewport(0, 0, 512, 512);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_brdfLookupProgram->Use();
    m_brdfLookupProgram->SetUniform("transform",
        glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, -2.0f, 2.0f)));
    m_plane->Draw(m_brdfLookupProgram.get());

    Framebuffer::BindToDefault();
    glViewport(0, 0, m_width, m_height);

    m_skyboxProgram = Program::Create(
        "./shader/skybox_hdr.vs", "./shader/skybox_hdr.fs");

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
            program->SetUniform("material.roughness", (float)(i + 1) / (float)sphereCount);
            program->SetUniform("material.metallic", (float)(j + 1) / (float)sphereCount);
            m_sphere->Draw(program);
        }
    }
}
