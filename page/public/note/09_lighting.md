# Lighting

---

## Lighting

- 물체 표면의 색상을 결정하는 과정
  - 광원 (light source)
  - 재질 (material)
  - 매우 복잡한 물리적 현상

<div>
<img src="/opengl_course/note/images/09_simple_light_diagram.png" style="width: 40%"/>
</div>

---

## Illumination Model

- Local / Global Illumination Model
  - 반사광: 광원이 물체에 반사되어 발생한 빛
  - Local: 반사광을 고려하지 않는다
  - Global: 반사광을 고려한다
    - 더 사실적, 높은 계산 비용

---

## Phong's Illumination Model

- 가장 대표적인 Local illumination model
  - 빛과 물체 간의 색상 결정을 3가지로 나눠서 표현
  - ambient (주변광), diffuse (분산광), specular (반사광)

![](/opengl_course/note/images/09_phong_illumination_model.png)

---

## Ambient light

---

## Diffuse light

---

## Specular light

---

## 예제 준비

- `imgui_example` 프로젝트를 복사하여 `lighting_example` 생성
- 프로젝트 이름 변경 후 빌드 및 테스트

---

## 예제 준비

- `shader/lighting.vs` 작성

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;

out vec3 normal;
out vec2 texCoord;

void main() {
  gl_Position = transform * vec4(aPos, 1.0);
  normal = aNormal;
  texCoord = aTexCoord;
}
```

---

## 예제 준비

- `shader/lighting.fs` 작성

```glsl
#version 330 core
in vec3 normal;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float ambientStrength;

void main() {
  vec3 ambient = ambientStrength * lightColor;
  vec3 result = ambient * objectColor;
  fragColor = vec4(result, 1.0);
}
```

---

## Ambient Light

- `lightColor`로 빛의 색상을, `objectColor`로 물체의 색상을 나타냄
- `ambientStrength` 만큼 전체 밝기를 높여줌

```glsl
vec3 ambient = ambientStrength * lightColor;
```

---

## Ambient Light

- `Context::Init()`에서 새로운 shader 로드

```cpp
ShaderPtr vertShader = Shader::CreateFromFile(
  "./shader/lighting.vs", GL_VERTEX_SHADER);
ShaderPtr fragShader = Shader::CreateFromFile(
  "./shader/lighting.fs", GL_FRAGMENT_SHADER);
```

---

## Ambient Light

- `Program` 클래스에 `float`, `vec3` 타입의 uniform 설정 함수 추가

```cpp [3-4]
  // in declaration of Program
  void SetUniform(const std::string& name, int value) const;
  void SetUniform(const std::string& name, float value) const;
  void SetUniform(const std::string& name, const glm::vec3& value) const;
  void SetUniform(const std::string& name, const glm::mat4& value) const;
```

```cpp
// program.cpp
void Program::SetUniform(const std::string& name, float value) const {
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1f(loc, value);
}

void Program::SetUniform(const std::string& name, const glm::vec3& value) const {
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(value));
}
```

---

## Ambient Light

- `Context` 클래스에 light color, object color 저장을 위한 멤버 추가

```cpp [4-7]
  // clear color
  glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };

  // light parameter
  glm::vec3 m_lightColor { glm::vec3(1.0f, 1.0f, 1.0f) };
  glm::vec3 m_objectColor { glm::vec3(1.0f, 0.5f, 0.0f) };
  float m_ambientStrength { 0.1f };
```

---

## Ambient Light

- `Context::Init()`에서 새로운 shader 로드

```cpp
ShaderPtr vertShader = Shader::CreateFromFile(
  "./shader/lighting.vs", GL_VERTEX_SHADER);
ShaderPtr fragShader = Shader::CreateFromFile(
  "./shader/lighting.fs", GL_FRAGMENT_SHADER);
```

---

## Ambient Light

- `Context::Render()`에서 ImGui를 이용한 파라미터 수정

```cpp [2-6]
if (ImGui::Begin("ui window")) {
  if (ImGui::CollapsingHeader("light")) {
    ImGui::ColorEdit3("light color", glm::value_ptr(m_lightColor));
    ImGui::ColorEdit3("object color", glm::value_ptr(m_objectColor));
    ImGui::SliderFloat("ambient strength", &m_ambientStrength, 0.0f, 1.0f);
  }
```

---

## Ambient Light

- shader에 uniform 적용

```cpp [3-6]
ImGui::End();

m_program->Use();
m_program->SetUniform("lightColor", m_lightColor);
m_program->SetUniform("objectColor", m_objectColor);
m_program->SetUniform("ambientStrength", m_ambientStrength);
```

---

## Ambient Light

- 빌드 및 실행 결과
  - 모든 픽셀이 광원의 위치, 방향에 상관없이 균일한 밝기로 렌더링

<div>
<img src="/opengl_course/note/images/09_ambient_light.png" style="width: 75%"/>
</div>

---

## Diffuse Light

- 분산광의 밝기를 결정하는 요소
  - 빛의 방향과 밝기
  - 표면의 법선 방향

---

## Diffuse Light

- 큐브의 정점 정보 추가

```cpp
float vertices[] = { // pos.xyz, normal.xyz, texcoord.uv
  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,

  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
};
```

---

## Diffuse Light

- VBO / VAO 수정

```cpp
m_vertexBuffer = Buffer::CreateWithData(
    GL_ARRAY_BUFFER, GL_STATIC_DRAW,
    vertices, sizeof(float) * 8 * 6 * 4);

m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE,
  sizeof(float) * 8, 0);
m_vertexLayout->SetAttrib(1, 2, GL_FLOAT, GL_FALSE,
  sizeof(float) * 8, sizeof(float) * 3);
m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE,
  sizeof(float) * 8, sizeof(float) * 6);
```

---

## Congratulation!
### 수고하셨습니다!
