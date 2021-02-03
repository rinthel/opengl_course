#include "program.h"

ProgramUPtr Program::Create(
    const std::vector<ShaderPtr>& shaders) {
    auto program = ProgramUPtr(new Program());
    if (!program->Link(shaders))
        return nullptr;
    return std::move(program);
}

ProgramUPtr Program::Create(
    const std::string& vertShaderFilename,
    const std::string& fragShaderFilename) {
    ShaderPtr vs = Shader::CreateFromFile(vertShaderFilename, GL_VERTEX_SHADER);
    ShaderPtr fs = Shader::CreateFromFile(fragShaderFilename, GL_FRAGMENT_SHADER);
    if (!vs || !fs)
        return nullptr;
    return std::move(Create({vs, fs}));
}

bool Program::Link(
    const std::vector<ShaderPtr>& shaders) {
    m_program = glCreateProgram();
    for (auto& shader: shaders)
        glAttachShader(m_program, shader->Get());
    glLinkProgram(m_program);

    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
        SPDLOG_ERROR("failed to link program: {}", infoLog);
        return false;
    }

    return true;
}

void Program::Use() const {
    glUseProgram(m_program);
}

Program::~Program() {
    if (m_program) {
        glDeleteProgram(m_program);
    }
}

void Program::SetUniform(const std::string& name, int value) const {
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1i(loc, value);
}

void Program::SetUniform(const std::string& name, float value) const {
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1f(loc, value);
}

void Program::SetUniform(const std::string& name, const glm::vec2& value) const {
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform2fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string& name, const glm::vec3& value) const {
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string& name, const glm::vec4& value) const {
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform4fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string& name, const glm::mat4& value) const {
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}
