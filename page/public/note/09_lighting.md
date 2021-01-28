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
  - 표면의 normal 방향

---

## Diffuse Light

- 큐브의 정점 데이터에 normal 추가

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
m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE,
  sizeof(float) * 8, sizeof(float) * 3);
m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE,
  sizeof(float) * 8, sizeof(float) * 6);
```

---

## Diffuse Light

- `shader/lighting.vs` 수정

```glsl [7,11,15,17]
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 modelTransform;

out vec3 normal;
out vec2 texCoord;
out vec3 position;

void main() {
  gl_Position = transform * vec4(aPos, 1.0);
  normal = (transpose(inverse(modelTransform)) * vec4(aNormal, 0.0)).xyz;
  texCoord = aTexCoord;
  position = (modelTransform * vec4(aPos, 1.0)).xyz;
}
```

---

## Diffuse Light

- `gl_Position`외에도 `position`을 계산하여 fragment shader에 넘기는 이유
  - `gl_Position`은 perspective transform이 적용되어 있어
    canonical space 상의 좌표값으로 전환됨
  - diffuse 값을 계산하려면 world space 상에서의 좌표값이 필요
  - `normal`도 같은 이유로 model transform만 적용

---

## Diffuse Light

- `normal`에 `modelTransform`의 inverse transpose를 적용하는 이유
  - 점이 아닌 벡터의 경우 이렇게 해야 제대로 변환된 값을 계산할 수 있음
  - matrix의 inverse transpose는 모든 점에서 동일하므로 보통 별도의 uniform으로
    입력함

<div>
<img src="/opengl_course/note/images/09_normal_transform.png" style="width: 30%"/>
</div>

---

## Diffuse Light

- `shader/lighting.fs` 구현 변경

```glsl [4,7,14-17]
#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float ambientStrength;

void main() {
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = normalize(lightPos - position);
    vec3 pixelNorm = normalize(normal);
    vec3 diffuse = max(dot(pixelNorm, lightDir), 0.0) * lightColor;
    vec3 result = (ambient + diffuse) * objectColor;
    fragColor = vec4(result, 1.0);
}
```

---

## Diffuse Light

- (빛의 방향) = (광원 위치) - (각 픽셀의 3D 위치)
- `normal`을 다시 normalize 하는 이유
  - vertex shader에서 계산된 `normal`은 rasterization 되는
    과정에서 선형 보간이 진행됨
  - unit vector 간의 선형 보간 결과는 unit vector 보장을 못하기
    때문에 normalization해야함

---

## Diffuse Light

- `Context` 클래스에 light position 관련 멤버 변수 추가
  - light의 위치에 따라 변하는 표면 색상을 관찰하기 위해 애니메이션
    멈춤기능도 추가

```cpp [1-2,7-8]
  // animation
  bool m_animation { true };

  // clear color
  glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };

  // light parameter
  glm::vec3 m_lightPos { glm::vec3(3.0f, 3.0f, 3.0f) };
  glm::vec3 m_lightColor { glm::vec3(1.0f, 1.0f, 1.0f) };
  glm::vec3 m_objectColor { glm::vec3(1.0f, 0.5f, 0.0f) };
  float m_ambientStrength { 0.1f };
```

---

## Diffuse Light

- `Context::Render()`에서 광원 위치 및 애니메이션 on/off UI 추가

```cpp [2-3,9]
if (ImGui::Begin("ui window")) {
  if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::DragFloat3("light pos", glm::value_ptr(m_lightPos), 0.01f);
    ImGui::ColorEdit3("light color", glm::value_ptr(m_lightColor));
    ImGui::ColorEdit3("object color", glm::value_ptr(m_objectColor));
    ImGui::SliderFloat("ambient strength", &m_ambientStrength, 0.0f, 1.0f);
  }

  ImGui::Checkbox("animation", &m_animation);

  if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))) {
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
  }
```

---

## Diffuse Light

- `lightPos`, `modelTransform` uniform 입력
- `m_animation` 값에 따라 회전 애니메이션 적용

```cpp [1]
m_program->SetUniform("lightPos", m_lightPos);
m_program->SetUniform("lightColor", m_lightColor);
m_program->SetUniform("objectColor", m_objectColor);
m_program->SetUniform("ambientStrength", m_ambientStrength);
```

```cpp [4-7,10]
for (size_t i = 0; i < cubePositions.size(); i++){
  auto& pos = cubePositions[i];
  auto model = glm::translate(glm::mat4(1.0f), pos);
  auto angle = glm::radians((float)glfwGetTime() * 120.0f + 20.0f * (float)i);
  model = glm::rotate(model,
    m_animation ? angle : 0.0f,
    glm::vec3(1.0f, 0.5f, 0.0f));
  auto transform = projection * view * model;
  m_program->SetUniform("transform", transform);
  m_program->SetUniform("modelTransform", model);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
```

---

## Diffuse Light

- light position에 light color로 작은 큐브를 그리자

```cpp
// after computing projection and view matrix
auto lightModelTransform =
  glm::translate(glm::mat4(1.0), m_lightPos) *
  glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
m_program->Use();
m_program->SetUniform("lightPos", m_lightPos);
m_program->SetUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
m_program->SetUniform("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
m_program->SetUniform("ambientStrength", 1.0f);
m_program->SetUniform("transform", projection * view * lightModelTransform);
m_program->SetUniform("modelTransform", lightModelTransform);
glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
```

---

## Diffuse Light

- 빌드 및 결과 확인
  - UI로 파라미터를 변경하면서 광원의 위치에 따라 명암이 생기는 것을 관찰

<div>
<img src="/opengl_course/note/images/09_diffuse_light.png" style="width: 70%"/>
</div>

---

## Congratulation!
### 수고하셨습니다!
