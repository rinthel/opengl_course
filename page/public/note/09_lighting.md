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
  - 위 3개의 빛을 더하여 최종 색상을 결정

![](/opengl_course/note/images/09_phong_illumination_model.png)

---

## Ambient light

- 주변광
- 빛의 방향, 물체 표면의 방향, 시선 방향과 아무 상관 없이 물체가 기본적으로 받는 빛
- 상수값으로 처리

---

## Diffuse light

- 분산광
- 빛이 물체 표면에 부딪혔을 때, **모든 방향으로 고르게 퍼지는 빛**
  - 즉 시선의 방향과는 상관없이 빛의 방향과 물체 표면의 방향에 따라 결정
- `diffuse = dot(light, normal)`

<div>
<img src="/opengl_course/note/images/09_diffuse_light_model.png" style="width: 30%"/>
</div>

---

## Specular light

- 반사광
- 빛이 물체 표면에 부딪혀 반사되는 광원
- 시선이 방향이 이 반사광의 방향과 동일할 때 가장 강한 빛이 나온다
- `reflect = 2 * dot(light, normal) - light`
- `specular = dot(view, reflect)`

<div>
<img src="/opengl_course/note/images/09_specular_light_model.png" style="width: 30%"/>
</div>

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

## Specular Light

- 반사광의 밝기를 결정하는 요소
  - 빛의 방향과 밝기
  - 표면의 normal 방향
  - **시선의 방향**

---

## Specular Light

- `shader/lighting.fs` 수정

```glsl [11-13, 22-25, 27]
#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform float specularShininess;
uniform vec3 viewPos;

void main() {
    vec3 ambient = ambientStrength * lightColor;

    vec3 lightDir = normalize(lightPos - position);
    vec3 pixelNorm = normalize(normal);
    vec3 diffuse = max(dot(pixelNorm, lightDir), 0.0) * lightColor;

    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularShininess);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    fragColor = vec4(result, 1.0);
}
```

---

## Specular Light

- `reflect(light, normal)` 함수
  - `light` 벡터 방향의 광선이 `normal` 벡터 방향의 표면에 부딪혔을 때 반사되는
    벡터를 출력하는 내장함수
- 현재 카메라의 world space 상의 좌표와 픽셀 좌표 간의 차를 통해
  시선 벡터 `viewDir` 계산
- `reflectDir`과 `viewDir` 간의 내적을 통해 반사광을 많이 보는 정도를 계산
- `specularStrength`로 반사광의 정도를 조절
- `specularShininess`로 반사광의 면적 조절

---

## Specualr Light

- `specularShininess` 값에 따른 specular 하이라이트의 변화

![](/opengl_course/note/images/09_specular_shininess.png)

---

## Specular Light

- `Context` 클래스에 반사광 제어를 위한 파라미터 추가

```cpp [6-7]
// light parameter
glm::vec3 m_lightPos { glm::vec3(3.0f, 3.0f, 3.0f) };
glm::vec3 m_lightColor { glm::vec3(1.0f, 1.0f, 1.0f) };
glm::vec3 m_objectColor { glm::vec3(1.0f, 0.5f, 0.0f) };
float m_ambientStrength { 0.1f };
float m_specularStrength { 0.5f };
float m_specularShininess { 32.0f };
```

---

## Specular Light

- `Context::Render()`에서 uniform 추가 세팅

```cpp [2-3]
ImGui::SliderFloat("ambient strength", &m_ambientStrength, 0.0f, 1.0f);
ImGui::SliderFloat("specular strength", &m_specularStrength, 0.0f, 1.0f);
ImGui::DragFloat("specular shininess", &m_specularShininess, 1.0f, 1.0f, 256.0f);
```

```cpp [1,6-7]
m_program->SetUniform("viewPos", m_cameraPos);
m_program->SetUniform("lightPos", m_lightPos);
m_program->SetUniform("lightColor", m_lightColor);
m_program->SetUniform("objectColor", m_objectColor);
m_program->SetUniform("ambientStrength", m_ambientStrength);
m_program->SetUniform("specularStrength", m_specularStrength);
m_program->SetUniform("specularShininess", m_specularShininess);
```

---

## Specular Light

- 빌드 및 결과
  - 시점과 광원의 위치, 파라미터를 바꿔가면서 specular highlight가 생기는
    모습을 관찰해보자

<div>
<img src="/opengl_course/note/images/09_specular_light.png" style="width: 60%"/>
</div>

---

## Material

- 최종 색상 = 빛 색상 * 재질의 색상
- 재질 (material)을 ambient / diffuse / specular로 나누어 표현
  - 금속 재질은 반사광이 강하다
  - 천 재질은 반사광이 약하다

<div>
<img src="/opengl_course/note/images/09_material_examples.png" style="width: 60%"/>
</div>

---

## Material

- `shader/lighting.fs` 수정

```glsl [9-23, 26-38]
#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 viewPos;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

void main() {
    vec3 ambient = material.ambient * light.ambient;

    vec3 lightDir = normalize(light.position - position);
    vec3 pixelNorm = normalize(normal);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * light.diffuse;

    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * light.specular;

    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}
```

---

## Material

- `Context` 클래스 내의 광원 / 재질 파라미터 수정

```cpp
// light parameter
struct Light {
  glm::vec3 position { glm::vec3(3.0f, 3.0f, 3.0f) };
  glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
  glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
  glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
};
Light m_light;

// material parameter
struct Material {
  glm::vec3 ambient { glm::vec3(1.0f, 0.5f, 0.3f) };
  glm::vec3 diffuse { glm::vec3(1.0f, 0.5f, 0.3f) };
  glm::vec3 specular { glm::vec3(0.5f, 0.5f, 0.5f) };
  float shininess { 32.0f };
};
Material m_material;
```

---

## Material

- `Context::Render()`에서 파라미터 UI 수정

```cpp [2-14]
if (ImGui::Begin("ui window")) {
  if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
      ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
      ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
      ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
  }

  if (ImGui::CollapsingHeader("material", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::ColorEdit3("m.ambient", glm::value_ptr(m_material.ambient));
      ImGui::ColorEdit3("m.diffuse", glm::value_ptr(m_material.diffuse));
      ImGui::ColorEdit3("m.specular", glm::value_ptr(m_material.specular));
      ImGui::DragFloat("m.shininess", &m_material.shininess, 1.0f, 1.0f, 256.0f);
  }
```

---

## Material

- `Context::Render()`의 uniform 적용 코드 수정

```cpp [2-9]
m_program->SetUniform("viewPos", m_cameraPos);
m_program->SetUniform("light.position", m_light.position);
m_program->SetUniform("light.ambient", m_light.ambient);
m_program->SetUniform("light.diffuse", m_light.diffuse);
m_program->SetUniform("light.specular", m_light.specular);
m_program->SetUniform("material.ambient", m_material.ambient);
m_program->SetUniform("material.diffuse", m_material.diffuse);
m_program->SetUniform("material.specular", m_material.specular);
m_program->SetUniform("material.shininess", m_material.shininess);
```

---

## Material

- 빌드 및 결과

![](/opengl_course/note/images/09_material_light.png)

---

## Lighting Maps

- 재질을 구성하는 ambient, diffuse, specular 모두 텍스처 맵으로 대체 가능
- 우리가 흔히 사용하는 오브젝트의 texture map은 결국 diffuse map으로 사용

---

## Lighting Maps

- 두 개의 텍스처 이미지 다운로드 후 `image` 디렉토리에 저장
  - [container2.png](https://learnopengl.com/img/textures/container2.png)
  - [container2_specular.png](https://learnopengl.com/img/textures/container2_specular.png)

<div>
<img src="https://learnopengl.com/img/textures/container2.png" style="width: 30%"/>
<img src="https://learnopengl.com/img/textures/container2_specular.png" style="width: 30%"/>
</div>

---

## Lighting Maps

- 여러 프로그램을 쉽게 초기화 하기 위한 리팩토링

```cpp [7-9, 12]
CLASS_PTR(Program)
class Program {
public:
  static ProgramUPtr Create(
    const std::vector<ShaderPtr>& shaders);
  
  static ProgramUPtr Create(
    const std::string& vertShaderFilename,
    const std::string& fragShaderFilename);

  // ...
  void SetUniform(const std::string& name, const glm::vec4& value) const;
  // ...
```

---

## Lighting Maps

- 여러 프로그램을 쉽게 초기화 하기 위한 리팩토링

```cpp
ProgramUPtr Program::Create(
  const std::string& vertShaderFilename,
  const std::string& fragShaderFilename) {
  ShaderPtr vs = Shader::CreateFromFile(vertShaderFilename,
    GL_VERTEX_SHADER);
  ShaderPtr fs = Shader::CreateFromFile(fragShaderFilename,
    GL_FRAGMENT_SHADER);
  if (!vs || !fs)
    return nullptr;
  return std::move(Create({vs, fs}));
}

void Program::SetUniform(const std::string& name,
  const glm::vec4& value) const {
  auto loc = glGetUniformLocation(m_program, name.c_str());
  glUniform4fv(loc, 1, glm::value_ptr(value));
}
```

---

## Lighting Maps

- `Context` 클래스에 `Program` 멤버 변수 추가

```cpp [5]
private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;
    ProgramUPtr m_simpleProgram;
```

---

## Lighting Maps

- `Context::Init()`에서 두 `Program` 초기화

```cpp
m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
if (!m_simpleProgram)
  return false;

m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
if (!m_program)
  return false;
```

---

## Lighting Maps

- `shader/simple.vs`, `shader/simple.fs` 수정

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
}
```

```glsl
#version 330 core
uniform vec4 color;
out vec4 fragColor;

void main() {
    fragColor = color;
}
```

---

## Lighting Maps

- `Context::Render()`에서 광원의 위치 그리는 코드를 수정

```cpp [4-9]
auto lightModelTransform =
    glm::translate(glm::mat4(1.0), m_light.position) *
    glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
m_simpleProgram->Use();
m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

m_program->Use();
```

---

## Lighting Maps

- `shader/lighting.fs` 수정

```glsl [2,9-10,15]
struct Material {
    sampler2D diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

void main() {
  vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
  vec3 ambient = texColor * light.ambient;

  vec3 lightDir = normalize(light.position - position);
  vec3 pixelNorm = normalize(normal);
  float diff = max(dot(pixelNorm, lightDir), 0.0);
  vec3 diffuse = diff * texColor * light.diffuse;
```

---

## Lighting Maps

- `Context` 내 `Material` 구조체 수정

```cpp [3]
  // material parameter
  struct Material {
      TextureUPtr diffuse;
      glm::vec3 specular { glm::vec3(0.5f, 0.5f, 0.5f) };
      float shininess { 32.0f };
  };
  Material m_material;
```

---

## Lighting Maps

- `Context::Init()`에서 diffuse 텍스처 로딩

```cpp
m_material.diffuse = Texture::CreateFromImage(
  Image::Load("./image/container2.png").get()
  );
```

---

## Lighting Maps

- `Context::Render()`에서 uniform 설정

```cpp [7,11-12]
m_program->Use();
m_program->SetUniform("viewPos", m_cameraPos);
m_program->SetUniform("light.position", m_light.position);
m_program->SetUniform("light.ambient", m_light.ambient);
m_program->SetUniform("light.diffuse", m_light.diffuse);
m_program->SetUniform("light.specular", m_light.specular);
m_program->SetUniform("material.diffuse", 0);
m_program->SetUniform("material.specular", m_material.specular);
m_program->SetUniform("material.shininess", m_material.shininess);

glActiveTexture(GL_TEXTURE0);
m_material.diffuse->Bind();
```

---

## Lighting Maps

- 빌드 및 실행 결과
  - 명암이 들어간 텍스처
  - 나무 부분이 금속 부분과 동일하게 반짝인다

<div>
<img src="/opengl_course/note/images/09_diffuse_map.png" style="width: 60%"/>
</div>

---

## Lighting Maps

- specular map
  - diffuse color과 더불어 specular color도 texture map으로 대체하자

```glsl [3]
struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};
```

```glsl [1,5]
vec3 specColor = texture2D(material.specular, texCoord).xyz;
vec3 viewDir = normalize(viewPos - position);
vec3 reflectDir = reflect(-lightDir, pixelNorm);
float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
vec3 specular = spec * specColor * light.specular;
```

---

## Lighting Maps

- `Context` 코드 수정

```cpp [4]
// ... in Context declaration
struct Material {
  TextureUPtr diffuse;
  TextureUPtr specular;
  float shininess { 32.0f };
};
```

```cpp [6-8]
// ... in Context::Init()
m_material.diffuse = Texture::CreateFromImage(
  Image::Load("./image/container2.png").get()
  );

m_material.specular = Texture::CreateFromImage(
  Image::Load("./image/container2_specular.png").get()
  );
```

---

## Lighting Maps

- `Context::Render()` 코드 수정

```cpp [8,13-14]
m_program->Use();
m_program->SetUniform("viewPos", m_cameraPos);
m_program->SetUniform("light.position", m_light.position);
m_program->SetUniform("light.ambient", m_light.ambient);
m_program->SetUniform("light.diffuse", m_light.diffuse);
m_program->SetUniform("light.specular", m_light.specular);
m_program->SetUniform("material.diffuse", 0);
m_program->SetUniform("material.specular", 1);
m_program->SetUniform("material.shininess", m_material.shininess);

glActiveTexture(GL_TEXTURE0);
m_material.diffuse->Bind();
glActiveTexture(GL_TEXTURE1);
m_material.specular->Bind();
```

---

## Lighting Maps

- 빌드 및 실행 결과
  - 나무 재질에는 하이라이트가 생기지 않음
  - 금속 재질에는 하이라이트 발생

<div>
<img src="/opengl_course/note/images/09_specular_map.png" style="width: 60%"/>
</div>

---

## Light Casters

- 물체에 빛을 쐬는 광원의 종류를 다양화하자
- 흔히 사용되는 3가지 종류의 light caster
  - Directional light
  - Point light (omni light)
  - Spot light

---

## Directional Light

- Directional light
  - 광원이 매우 멀리 떨어져 있어서 모든 지점에 동일한 방향의 광선이 평행하게 발사
  - 태양광

<div>
<img src="/opengl_course/note/images/09_directional_light.png" style="width: 40%"/>
</div>

---

## Directional Light

- 현재의 light caster를 directional light로 바꿔보자

```glsl [2]
struct Light {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
```

```glsl [5]
void main() {
  vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
  vec3 ambient = texColor * light.ambient;

  vec3 lightDir = normalize(-light.direction);
```

---

## Directional Light

- `Context` 클래스 내의 light parameter 변경

```cpp [2]
struct Light {
  glm::vec3 direction { glm::vec3(-0.2f, -1.0f, -0.3f) };
  glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
  glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
  glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
};
```

---

## Directional Light

- `Context::Render()` 코드 수정
  - 광원 위치를 그리는 코드는 주석처리해두자

```cpp [3]
m_program->Use();
m_program->SetUniform("viewPos", m_cameraPos);
m_program->SetUniform("light.direction", m_light.direction);
m_program->SetUniform("light.ambient", m_light.ambient);
m_program->SetUniform("light.diffuse", m_light.diffuse);
m_program->SetUniform("light.specular", m_light.specular);
```

---

## Directional Light

- 빌드 및 실행 결과
  - 모든 위치의 상자에 대해 동일한 방향의 빛이 적용

<div>
<img src="/opengl_course/note/images/09_directional_light_apply.png" style="width: 60%"/>
</div>

---

## Point Light

- Point light
  - 한 점에서부터 모든 방향으로 광선이 발사
    - Omni direction
    - Directional light 이전의 예제는 모두 Point light로 볼 수 있음
  - 전구

<div>
<img src="/opengl_course/note/images/09_point_light.png" style="width: 40%"/>
</div>

---

## Point Light

- Attenuation
  - 빛의 감쇠
  - 물리 법칙으로는 거리의 제곱에 반비례하게 빛의 감쇠가 일어남
  - 그러나 local illumination model, 즉 주변의 반사광을 고려하지
    않는 경우 거리의 제곱에 직접 반비례하게 만들 경우 급격히 어두워짐

---

## Point Light

- Attenumation model
  - d: 광원과 물체 표면 간의 거리
  - Kc, Kl, Kq 세 개의 파라미터
  - 광원이 어느정도 거리까지 영향을 주게할 것인지에 따라 파라미터를 조절

<div>
<img src="/opengl_course/note/images/09_attenuation_equation.png" style="width: 30%"/>
<img src="/opengl_course/note/images/09_attenuation_graph.png" style="width: 40%"/>
</div>

---

## Point Light

- Ogre3D 엔진에서 사용하는 점 광원 최대 거리에 따른 파라미터

<div>
<img src="/opengl_course/note/images/09_attenuation_parameter_table.png" style="width: 50%"/>
</div>

---

## Point Light

- `shader/lighting.fs` 수정

```glsl [2-3]
struct Light {
    vec3 position;
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
```

```glsl [1-4,9]
float dist = length(light.position - position);
vec3 distPoly = vec3(1.0, dist, dist*dist);
float attenuation = 1.0 / dot(distPoly, light.attenuation);
vec3 lightDir = (light.position - position) / dist;
vec3 pixelNorm = normalize(normal);

// ...

vec3 result = (ambient + diffuse + specular) * attenuation;
```

---

## Point Light

- 최대 거리에 따라 attenuation coefficient를 근사해주는 함수를
  `src/common.*`에 정의 및 구현

```cpp
glm::vec3 GetAttenuationCoeff(float distance) {
  const auto linear_coeff = glm::vec4(
    8.4523112e-05, 4.4712582e+00, -1.8516388e+00, 3.3955811e+01
  );
  const auto quad_coeff = glm::vec4(
    -7.6103583e-04, 9.0120201e+00, -1.1618500e+01, 1.0000464e+02
  );

  float kc = 1.0f;
  float d = 1.0f / distance;
  auto dvec = glm::vec4(1.0f, d, d*d, d*d*d);
  float kl = glm::dot(linear_coeff, dvec);
  float kq = glm::dot(quad_coeff, dvec);
  
  return glm::vec3(kc, glm::max(kl, 0.0f), glm::max(kq*kq, 0.0f));
}
```

---

## Point Light

- `Context` 클래스 내의 light parameter 변경

```cpp [3-4]
// light parameter
struct Light {
  glm::vec3 position { glm::vec3(2.0f, 2.0f, 2.0f) };
  float distance { 32.0f };
  glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
  glm::vec3 diffuse { glm::vec3(0.8f, 0.8f, 0.8f) };
  glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
};
Light m_light;
```

---

## Point Light

- `Context::Render()`의 uniform 적용 코드 변경
  - position, distance 수정이 가능하도록 UI 코드를 고쳐보자
  - 주석을 제거하여 광원 위치를 다시 그려보자

```cpp [3-5]
m_program->Use();
m_program->SetUniform("viewPos", m_cameraPos);
m_program->SetUniform("light.position", m_light.position);
m_program->SetUniform("light.attenuation",
  GetAttenuationCoeff(m_light.distance));
m_program->SetUniform("light.ambient", m_light.ambient);
m_program->SetUniform("light.diffuse", m_light.diffuse);
m_program->SetUniform("light.specular", m_light.specular);
```

---

## Point Light

- 빌드 및 결과
  - 점광원으로부터 거리가 멀수록 상자가 어두워 지는 것을 확인

<div>
<img src="/opengl_course/note/images/09_point_light_apply.png" style="width: 60%"/>
</div>

---

## Spot Light

- Spot light
  - 스포트라이트
  - 광원의 위치와 방향 모두를 가지고 있음
  - 광원 방향으로부터 일정 각도만큼만 광선을 발사

<div>
<img src="/opengl_course/note/images/09_spot_light_model.png" style="width: 30%"/>
</div>

---

## Spot Light

- `shader/lighting.fs` 내의 light 관련 파라미터 추가

```glsl [3-4]
struct Light {
    vec3 position;
    vec3 direction;
    float cutoff;
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
```

---

## Spot Light

- `shader/lighting.fs` 로직 변경
  - `cutoff`를 벗어난 위치의 경우에는 ambient를 제외한 빛 적용 제외

```glsl
float theta = dot(lightDir, normalize(-light.direction));
vec3 result = ambient;

if (theta > light.cutoff) {
  vec3 pixelNorm = normalize(normal);
  float diff = max(dot(pixelNorm, lightDir), 0.0);
  vec3 diffuse = diff * texColor * light.diffuse;

  vec3 specColor = texture2D(material.specular, texCoord).xyz;
  vec3 viewDir = normalize(viewPos - position);
  vec3 reflectDir = reflect(-lightDir, pixelNorm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = spec * specColor * light.specular;

  result += diffuse + specular;
}

result *= attenuation;
```

---

## Spot Light

- `Context` 클래스 내의 light parameter 변경

```cpp [4-5]
// light parameter
struct Light {
  glm::vec3 position { glm::vec3(2.0f, 2.0f, 2.0f) };
  glm::vec3 direction { glm::vec3(-1.0f, -1.0f, -1.0f) };
  float cutoff { 20.0f };
  float distance { 32.0f };
  glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
  glm::vec3 diffuse { glm::vec3(0.8f, 0.8f, 0.8f) };
  glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
};
Light m_light;
```

---

## Spot Light

- `Context::Render()`에서 uniform 적용 코드 추가
  - ImGui를 이용하여 파라미터 변경도 가능하도록 해보자

```cpp [4-5]
m_program->Use();
m_program->SetUniform("viewPos", m_cameraPos);
m_program->SetUniform("light.position", m_light.position);
m_program->SetUniform("light.direction", m_light.direction);
m_program->SetUniform("light.cutoff", cosf(glm::radians(m_light.cutoff)));
m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
m_program->SetUniform("light.ambient", m_light.ambient);
m_program->SetUniform("light.diffuse", m_light.diffuse);
m_program->SetUniform("light.specular", m_light.specular);
```

---

## Spot Light

- 빌드 및 실행 결과
  - 광원의 방향을 기준으로 일정한 범위만 조명 적용
  - 매우 또렷한 경계선

<div>
<img src="/opengl_course/note/images/09_spot_light_apply.png" style="width: 60%"/>
</div>

---

## Spot Light

- 부드러운 경계선을 만드는 방법
  - 광원의 방향을 기준으로 바깥쪽 각도 / 안쪽 각도를 정한다
  - 안쪽 각도 내의 빛은 조명을 100% 적용
  - 안쪽에서 바깥쪽 각도 사이의 빛은 조명을 100%에서 0% 사이에 오게끔 조절
  - I = (cos(x) - cos(outer)) / (cos(inner) - cos(outer))

---

## Spot Light

- `shader/lighting.fs` 수정

```glsl [4]
struct Light {
  vec3 position;
  vec3 direction;
  vec2 cutoff;
  // ...
```

```glsl [3-7, 12]
vec3 result = ambient;
float theta = dot(lightDir, normalize(-light.direction));
float intensity = clamp(
  (theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]),
  0.0, 1.0);

if (intensity > 0.0) {
  vec3 pixelNorm = normalize(normal);

  // ...

  result += (diffuse + specular) * intensity;
```

---

## Spot Light

- `Program` 클래스에서 `glm::vec2`를 인자로 사용할 수 있는 `SetUniform` 함수 추가
- `Context` 클래스의 `cutoff` 멤버 변수 변경
  - inner cut-off angle, offset angle

```cpp [5]
// light parameter
struct Light {
  glm::vec3 position { glm::vec3(2.0f, 2.0f, 2.0f) };
  glm::vec3 direction { glm::vec3(-1.0f, -1.0f, -1.0f) };
  glm::vec2 cutoff { glm::vec2(20.0f, 5.0f) };
  // ...
```

---

## Spot Light

- `Context::Render()`의 uniform 설정 코드 수정

```cpp [5-7]
m_program->Use();
m_program->SetUniform("viewPos", m_cameraPos);
m_program->SetUniform("light.position", m_light.position);
m_program->SetUniform("light.direction", m_light.direction);
m_program->SetUniform("light.cutoff", glm::vec2(
  cosf(glm::radians(m_light.cutoff[0])),
  cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
```

---

## Spot Light

- 빌드 및 실행 결과
  - 부드러운 경계선

<div>
<img src="/opengl_course/note/images/09_spot_light_apply_smooth_edge.png" style="width: 80%"/>
</div>

---

## Multiple Lights

- 여러 개의 조명 적용
  - 복수 개의 광원 파라미터를 fragment shader 내에서 처리

```glsl
out vec4 FragColor;
void main() {
  // 결과 색상 초기화
  vec3 output = vec3(0.0);
  // directional light에 의한 결과 누적
  output += someFunctionToCalculateDirectionalLight();
  // 모든 point light 들에 대해 결과 누적
  for(int i = 0; i < nr_of_point_lights; i++)
    output += someFunctionToCalculatePointLight();
  // spot light에 대한 결과 누적
  output += someFunctionToCalculateSpotLight();
  FragColor = vec4(output, 1.0);
}
```

---

## Congratulation!
### 수고하셨습니다!
