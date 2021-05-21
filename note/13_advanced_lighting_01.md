# Advanced Lighing Part 1

---

## Blinn-Phong Shading

- Phong shading의 문제점
  - Specular shininess 값이 작을 경우, highlight 부분이 잘려지는 현상

<div>
<img src="/opengl_course/note/images/13_phong_side_effect.png" width="55%"/>
</div>

---

## Blinn-Phong Shading

- Phong shading의 문제점 원인
  - Specular를 계산할 때 view와 reflection 간의 각도가 90도보다 커지는 경우
  - dot product 값이 0보다 작아져 cutoff가 발생

<div>
<img src="/opengl_course/note/images/13_phong_side_effect_reason.png" width="75%"/>
</div>

---

## Blinn-Phong Shading

- Blinn의 제안한 방식
  - view와 light를 이등분하는 벡터 (halfway)와 normal 벡터 간의 사잇각으로 계산해보자
  - view가 reflection과 일치할때 halfway 벡터는 normal 벡터와 일치하게 된다

<div>
<img src="/opengl_course/note/images/13_blinn_phong_idea.png" width="37%"/>
</div>

---

## Blinn-Phong Shading

- Phong과 Blinn 방식의 결과 차이
  - shininess 값이 작을 경우

<div>
<img src="/opengl_course/note/images/13_blinn_phong_result.png" width="75%"/>
</div>

---

## Blinn-Phong Shading

- Phong과 Blinn 방식의 결과 차이
  - shininess 값이 클 경우

<div>
<img src="/opengl_course/note/images/13_blinn_phong_result_2.png" width="75%"/>
</div>

---

## Blinn-Phong Shading

- 예제 준비
  - `advanced_feature_example` 프로젝트를 복사하여 `advanced_lighting_example` 프로젝트 구성

---

## Blinn-Phong Shading

- `shader/lighting.fs` 구현 변경

```glsl
uniform int blinn;

// ...

vec3 specColor = texture2D(material.specular, texCoord).xyz;
float spec = 0.0;
if (blinn == 0) {
  vec3 viewDir = normalize(viewPos - position);
  vec3 reflectDir = reflect(-lightDir, pixelNorm);
  spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
}
else {
  vec3 viewDir = normalize(viewPos - position);
  vec3 halfDir = normalize(lightDir + viewDir);
  spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);
}
vec3 specular = spec * specColor * light.specular;
```

---

## Blinn-Phong Shading

- Context 클래스에 Blinn 사용 여부를 저장할 bool 타입 멤버를 추가
- ImGui로 조작하면서 변화를 관측해보자

```cpp
// Context.h
bool m_blinn { false };
```

```cpp
// ImGui UI
ImGui::Checkbox("l.blinn", &m_blinn);

// Drawing
m_program->SetUniform("blinn", (m_blinn ? 1 : 0));
```

---

## Shadow

- 입체감을 만드는 매우 중요한 요소

<div>
<img src="/opengl_course/note/images/13_shadow_comparison.png" width="75%"/>
</div>

---

## Shadow Map

- 가쟝 많이 사용되는 그림자 렌더링 알고리즘
- 구현 난이도가 어렵지 않은 편이면서 성능을 크게 잡아먹지 않는 편
- 고급 알고리즘으로의 확장이 편하다
  - omnidiractional shadow map
  - cascaded shadow map

---

## Shadow Map

- Idea: 빛의 시점에서 렌더링을 해본다
  - 빛이 볼 수 있는 부분은 그림자가 지지 않는다
  - 빛이 볼 수 없는 부분은 그림자가 진다

<div>
<img src="/opengl_course/note/images/13_shadow_map_idea.png" width="75%"/>
</div>

---

## Shadow Map

- 알고리즘
  - light source의 위치에서 물체들을 depth map만 렌더링한다 (shadow map)
  - shadow map을 바탕으로 지금 그리고 있는 픽셀이 빛을 받는지 아닌지 판별한다

<div>
<img src="/opengl_course/note/images/13_shadow_map_algorithm.png" width="60%"/>
</div>

---

## Shadow Map Scene

- 장면 준비
  - `Context::DrawScene()` 함수를 정의하여 장면을 그리는 부분을 분리하자

```cpp
// context.h
void DrawScene(const glm::mat4& view,
  const glm::mat4& projection,
  const Program* program);
```

---

## Shadow Map Scene

- `Context::DrawScene()` 구현

```cpp
// context.cpp
void Context::DrawScene(const glm::mat4& view,
  const glm::mat4& projection,
  const Program* program) {

  program->Use();
  auto modelTransform =
    glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) *
    glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
  auto transform = projection * view * modelTransform;
  program->SetUniform("transform", transform);
  program->SetUniform("modelTransform", modelTransform);
  m_planeMaterial->SetToProgram(program);
  m_box->Draw(program);

  modelTransform =
    glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
    glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
  transform = projection * view * modelTransform;
  program->SetUniform("transform", transform);
  program->SetUniform("modelTransform", modelTransform);
  m_box1Material->SetToProgram(program);
  m_box->Draw(program);

  modelTransform =
    glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
    glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
  transform = projection * view * modelTransform;
  program->SetUniform("transform", transform);
  program->SetUniform("modelTransform", modelTransform);
  m_box2Material->SetToProgram(program);
  m_box->Draw(program);

  modelTransform =
    glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 1.75f, -2.0f)) *
    glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
  transform = projection * view * modelTransform;
  program->SetUniform("transform", transform);
  program->SetUniform("modelTransform", modelTransform);
  m_box2Material->SetToProgram(program);
  m_box->Draw(program);
}
```

---

## Shadow Map Scene

- `Context::Render()` 함수에서 `Context::DrawScene()` 함수를 호출
  - 그 외의 렌더링 코드는 제거

```cpp [14]
m_program->Use();
m_program->SetUniform("viewPos", m_cameraPos);
m_program->SetUniform("light.position", m_light.position);
m_program->SetUniform("light.direction", m_light.direction);
m_program->SetUniform("light.cutoff", glm::vec2(
    cosf(glm::radians(m_light.cutoff[0])),
    cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
m_program->SetUniform("light.ambient", m_light.ambient);
m_program->SetUniform("light.diffuse", m_light.diffuse);
m_program->SetUniform("light.specular", m_light.specular);
m_program->SetUniform("blinn", (m_blinn ? 1 : 0));

DrawScene(view, projection, m_program.get());
```

---

## Shadow Map Scene

- 정리된 장면

<div>
<img src="/opengl_course/note/images/13_shadow_test_scene.png" width="60%"/>
</div>

---

## Shadow Map

- `Texture` 클래스 코드 리팩토링
  - pixel type의 파라미터화
    - 현재 `Texture` 클래스는 모든 픽셀을 `GL_UNSIGNED_BYTE`로 처리
    - depth map texture를 사용하기 위해서는 `GL_FLOAT` 타입을 사용해야함

---

## Shadow Map

- `Texture` 클래스 코드 리팩토링
  - pixel type의 파라미터화

```cpp
// texture.h
static TextureUPtr Create(int width, int height,
  uint32_t format, uint32_t type = GL_UNSIGNED_BYTE);
// ...
uint32_t GetType() const { return m_type; }
// ...
void SetTextureFormat(int width, int height, uint32_t format, uint32_t type);
// ...
uint32_t m_type { GL_UNSIGNED_BYTE };
```

---

## Shadow Map

- `Texture` 클래스 코드 리팩토링
  - pixel type의 파라미터화

```cpp [2, 5]
TextureUPtr Texture::Create(int width, int height,
  uint32_t format, uint32_t type) {
  auto texture = TextureUPtr(new Texture());
  texture->CreateTexture();
  texture->SetTextureFormat(width, height, format, type);
  texture->SetFilter(GL_LINEAR, GL_LINEAR);
  return std::move(texture);
}
```

---

## Shadow Map

```cpp [13, 17]
void Texture::SetTextureFromImage(const Image* image) {
  GLenum format = GL_RGBA;
  switch (image->GetChannelCount()) {
      default: break;
      case 1: format = GL_RED; break;
      case 2: format = GL_RG; break;
      case 3: format = GL_RGB; break;
  }

  m_width = image->GetWidth();
  m_height = image->GetHeight();
  m_format = format;
  m_type = GL_UNSIGNED_BYTE;
  
  glTexImage2D(GL_TEXTURE_2D, 0, m_format,
      m_width, m_height, 0,
      format, m_type,
      image->GetData());

  glGenerateMipmap(GL_TEXTURE_2D);
}
```

---

## Shadow Map

```cpp [2, 6, 10]
void Texture::SetTextureFormat(int width, int height,
  uint32_t format, uint32_t type) {
  m_width = width;
  m_height = height;
  m_format = format;
  m_type = type;

  glTexImage2D(GL_TEXTURE_2D, 0, m_format,
    m_width, m_height, 0,
    m_format, m_type,
    nullptr);
}
```

---

## Shadow Map

- `src/shadow_map.h` 추가

```cpp
#ifndef __SHADOW_MAP_H__
#define __SHADOW_MAP_H__

#include "texture.h"

CLASS_PTR(ShadowMap);
class ShadowMap {
public:
  static ShadowMapUPtr Create(int width, int height);
  ~ShadowMap();

  const uint32_t Get() const { return m_framebuffer; }
  void Bind() const;
  const TexturePtr GetShadowMap() const { return m_shadowMap; }

private:
  ShadowMap() {}
  bool Init(int width, int height);

  uint32_t m_framebuffer { 0 };
  TexturePtr m_shadowMap;
};

#endif // __SHADOW_MAP_H__
```

---

## Shadow Map

- `ShadowMap` 클래스 디자인
  - `m_shadowMap`: depth map 저장을 위한 텍스처 멤버
  - `m_framebuffer`: depth map에 렌더링을 하기 위한 프레임버퍼
  - shadow map의 크기를 지정하면 해당 크기의 depth 텍스처를 만들어서
    framebuffer에 바인딩하자
  - color / stencil은 사용하지 않음

---

## Shadow Map

- `src/shadow_map.cpp` 추가

```cpp
#include "shadow_map.h"

ShadowMapUPtr ShadowMap::Create(int width, int height) {
  auto shadowMap = ShadowMapUPtr(new ShadowMap());
  if (!shadowMap->Init(width, height))
    return nullptr;
  return std::move(shadowMap);
}

ShadowMap::~ShadowMap() {
  if (m_framebuffer) {
    glDeleteFramebuffers(1, &m_framebuffer);
  }
}

void ShadowMap::Bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool ShadowMap::Init(int width, int height) {
  glGenFramebuffers(1, &m_framebuffer);
  Bind();

  m_shadowMap = Texture::Create(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
  m_shadowMap->SetFilter(GL_NEAREST, GL_NEAREST);
  m_shadowMap->SetWrap(GL_REPEAT, GL_REPEAT);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    GL_TEXTURE_2D, m_shadowMap->Get(), 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    SPDLOG_ERROR("failed to complete shadow map framebuffer: {:x}", status);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return false;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return true;
}
```

---

## Shadow Map

- `ShadowMap` 클래스 구현
  - `Init()` 함수는 `Framebuffer`와 크게 다르지 않음
  - depth texture만 framebuffer에 바인딩
  - `glDrawBuffer(GL_NONE)`, `glReadBuffer(GL_NONE)`을
    명시적으로 호출
    - color attachment가 없음을 OpenGL에게 알려줌

---

## Shadow Map - First Pass

- `Context` 클래스에 `ShadowMap` 멤버 추가

```cpp
  // shadow map
  ShadowMapUPtr m_shadowMap;
```

---

## Shadow Map - First Pass

- `Context::Init()`에서 `ShadowMap` 초기화

```cpp
  m_shadowMap = ShadowMap::Create(1024, 1024);
```

---

## Shadow Map - First Pass

- `Context::Render()`에서 `ShadowMap` 프레임버퍼에 렌더링
  - 가장 단순한 형태의 shader를 사용, depth buffer만 채우기
  - light의 위치 및 방향을 사용하여 view / projection 행렬 계산
  - lighting shader를 이용하기 전에 그리도록 함

---

## Shadow Map - First Pass

```cpp
  auto lightView = glm::lookAt(m_light.position,
    m_light.position + m_light.direction,
    glm::vec3(0.0f, 1.0f, 0.0f));
  auto lightProjection = glm::perspective(
    glm::radians((m_light.cutoff[0] + m_light.cutoff[1]) * 2.0f),
    1.0f, 1.0f, 20.0f);

  m_shadowMap->Bind();
  glClear(GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0,
      m_shadowMap->GetShadowMap()->GetWidth(),
      m_shadowMap->GetShadowMap()->GetHeight());
  m_simpleProgram->Use();
  m_simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
  DrawScene(lightView, lightProjection, m_simpleProgram.get());

  Framebuffer::BindToDefault();
  glViewport(0, 0, m_width, m_height);
```

---

## Shadow Map - First Pass

- ImGui를 이용하여 그려진 shadow map을 비주얼라이징

```cpp
  ImGui::Image((ImTextureID)m_shadowMap->GetShadowMap()->Get(),
    ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
```

---

## Shadow Map - First Pass

- `Context` 클래스의 light 관련 초기 파라미터 변경

```cpp [3-6, 12]
  // light parameter
  struct Light {
    glm::vec3 position { glm::vec3(2.0f, 4.0f, 4.0f) };
    glm::vec3 direction { glm::vec3(-0.5f, -1.5f, -1.0f) };
    glm::vec2 cutoff { glm::vec2(50.0f, 5.0f) };
    float distance { 150.0f };
    glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
    glm::vec3 diffuse { glm::vec3(0.8f, 0.8f, 0.8f) };
    glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
  };
  Light m_light;
  bool m_blinn { true };
```

---

## Shadow Map - First Pass

- 빌드 및 실행 결과
  - light의 시점에서 그려진 depth map을 ImGui 윈도우에서 확인

<div>
<img src="/opengl_course/note/images/13_shadow_map_first_pass.png" width="60%"/>
</div>

---

## Shadow Map - Second Pass

- `shader/lighting_shadow.vs` 추가

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT {
  vec3 fragPos;
  vec3 normal;
  vec2 texCoord;
  vec4 fragPosLight;
} vs_out;

uniform mat4 transform;
uniform mat4 modelTransform;
uniform mat4 lightTransform;

void main() {
  gl_Position = transform * vec4(aPos, 1.0);
  vs_out.fragPos = vec3(modelTransform * vec4(aPos, 1.0));
  vs_out.normal = transpose(inverse(mat3(modelTransform))) * aNormal;
  vs_out.texCoord = aTexCoord;
  vs_out.fragPosLight = lightTransform * vec4(vs_out.fragPos, 1.0);
}
```

---

## Shadow Map - Second Pass

- vertex shader 구현 이슈
  - 대부분은 `lighting.vs`와 동일함
  - 픽셀의 3D 공간 상의 위치를 light를 기준으로 계산한 `fragPosLight`가 추가

---

## Shadow Map - Second Pass

- `shader/lighting_shadow.fs` 추가

```cpp
#version 330 core

out vec4 fragColor;

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    vec4 fragPosLight;
} fs_in;

uniform vec3 viewPos;
struct Light {
    vec3 position;
    vec3 direction;
    vec2 cutoff;
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;
uniform int blinn;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLight) {
  return 0.0;
}

void main() {
  vec3 texColor = texture2D(material.diffuse, fs_in.texCoord).xyz;
  vec3 ambient = texColor * light.ambient;

  float dist = length(light.position - fs_in.fragPos);
  vec3 distPoly = vec3(1.0, dist, dist*dist);
  float attenuation = 1.0 / dot(distPoly, light.attenuation);
  vec3 lightDir = (light.position - fs_in.fragPos) / dist;

  vec3 result = ambient;
  float theta = dot(lightDir, normalize(-light.direction));
  float intensity = clamp(
      (theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]),
      0.0, 1.0);

  if (intensity > 0.0) {
    vec3 pixelNorm = normalize(fs_in.normal);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * texColor * light.diffuse;

    vec3 specColor = texture2D(material.specular, fs_in.texCoord).xyz;
    float spec = 0.0;
    if (blinn == 0) {
        vec3 viewDir = normalize(viewPos - fs_in.fragPos);
        vec3 reflectDir = reflect(-lightDir, pixelNorm);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }
    else {
        vec3 viewDir = normalize(viewPos - fs_in.fragPos);
        vec3 halfDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);
    }
    vec3 specular = spec * specColor * light.specular;
    float shadow = ShadowCalculation(fs_in.fragPosLight);

    result += (diffuse + specular) * intensity * (1.0 - shadow);
  }

  result *= attenuation;
  fragColor = vec4(result, 1.0);
}
```

---

## Shadow Map - Second Pass

- fragment shader 구현 이슈
  - 대부분의 코드는 `lighting.fs`와 동일
  - `shadowMap` 멤버 추가
  - `ShadowCalculation()`을 호출하여 그림자가 있을 경우 (1.0)
    diffuse / specular 컬러를 적용하지 않도록 함
  - 현재 `ShadowCalculation()` 부분은 미구현

---

## Shadow Map - Second Pass

- `Context` 클래스에 shadow shader 멤버 추가

```cpp [3]
  // shadow map
  ShadowMapUPtr m_shadowMap;
  ProgramUPtr m_lightingShadowProgram;
```

---

## Shadow Map - Second Pass

- `Context::Init()` 함수에서 shader 초기화

```cpp
  m_lightingShadowProgram = Program::Create(
    "./shader/lighting_shadow.vs", "./shader/lighting_shadow.fs");
```

---

## Shadow Map - Second Pass

- `Context::Render()` 함수에서 기본 lighting shader 대신
  lighting shadow shader 사용

```cpp
  m_lightingShadowProgram->Use();
  m_lightingShadowProgram->SetUniform("viewPos", m_cameraPos);
  m_lightingShadowProgram->SetUniform("light.position", m_light.position);
  m_lightingShadowProgram->SetUniform("light.direction", m_light.direction);
  m_lightingShadowProgram->SetUniform("light.cutoff", glm::vec2(
      cosf(glm::radians(m_light.cutoff[0])),
      cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
  m_lightingShadowProgram->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
  m_lightingShadowProgram->SetUniform("light.ambient", m_light.ambient);
  m_lightingShadowProgram->SetUniform("light.diffuse", m_light.diffuse);
  m_lightingShadowProgram->SetUniform("light.specular", m_light.specular);
  m_lightingShadowProgram->SetUniform("blinn", (m_blinn ? 1 : 0));
  m_lightingShadowProgram->SetUniform("lightTransform", lightProjection * lightView);
  glActiveTexture(GL_TEXTURE3);
  m_shadowMap->GetShadowMap()->Bind();
  m_lightingShadowProgram->SetUniform("shadowMap", 3);
  glActiveTexture(GL_TEXTURE0);

  DrawScene(view, projection, m_lightingShadowProgram.get());
```

---

## Shadow Map - Second Pass

- 빌드 및 실행
  - 이전의 lighting shader와 결과가 동일한 것을 먼저 확인
  - `ShadowCalculation()`을 리턴값을 1.0로 변경하면 전부 어둡게 렌더링 되는 것을 확인

---

## Shadow Calculation

- `ShadowCalculation()` 구현
  - `fragPosLight`를 바탕으로 light에서 본 픽셀의 위치 계산
  - `shadowMap`에서부터 해당 픽셀의 depth값 가져오기
  - 저장된 depth값이 fragPosLight의 depth값보다 작은 경우
    그림자가 생긴 것으로 판단

---

## Shadow Calculation

```glsl
float ShadowCalculation(vec4 fragPosLight) {
  // perform perspective divide
  vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
  // transform to [0,1] range
  projCoords = projCoords * 0.5 + 0.5;
  // get closest depth value from light’s perspective (using
  // [0,1] range fragPosLight as coords)
  float closestDepth = texture(shadowMap, projCoords.xy).r;
  // get depth of current fragment from light’s perspective
  float currentDepth = projCoords.z;
  // check whether current frag pos is in shadow
  float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
  return shadow;
}
```

---

## Shadow Calcuation

- 빌드 및 실행
  - 첫 그림자 렌더링

<div>
<img src="/opengl_course/note/images/13_shadow_map_second_pass.png" width="60%"/>
</div>

---

## Shadow Map Improvement

- 현재 렌더링된 그림자의 문제
  - 이상한 줄무늬가 나타난다 => shadow acne

<div>
<img src="/opengl_course/note/images/13_shadow_acne.png" width="30%"/>
</div>

---

## Shadow Map Improvement

- Shadow acne 현상의 원인
  - shadow map의 해상도는 한정적이어서 광원으로부터 먼 픽셀들은
    동일한 depth값을 샘플링할 확률이 올라감
  - 정확하지 않은 depth값 때문에 줄무늬 그림자 형성

<div>
<img src="/opengl_course/note/images/13_shadow_acne_reason.png" width="60%"/>
</div>

---

## Shadow Map Improvement

- Shadow acne 현상의 해결책
  - 편향치(bias)를 더한 값을 비교 수치로 사용하자

<div>
<img src="/opengl_course/note/images/13_shadow_acne_reason.png" width="60%"/>
</div>

```glsl
  float bias = 0.005;
  float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
```

---

## Shadow Map Improvement

- 빌드 및 결과
  - 상당히 많은 shadow acne가 없어짐

<div>
<img src="/opengl_course/note/images/13_shadow_bias_fixed.png" width="60%"/>
</div>

---

## Shadow Map Improvement

- 빌드 및 결과
  - light direction과 surface normal이 크게 차이 나는 경우 여전히 shadow acne 발생

<div>
<img src="/opengl_course/note/images/13_shadow_bias_fixed_limitation.png" width="60%"/>
</div>

---

## Shadow Map Improvement

- Dynamic shadow bias
  - light direction과 surface normal 간의 각도가 크면 bias도 크게 만들자

```glsl
float ShadowCalculation(vec4 fragPosLight, vec3 normal, vec3 lightDir) {
  vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
  projCoords = projCoords * 0.5 + 0.5;
  float closestDepth = texture(shadowMap, projCoords.xy).r;
  float currentDepth = projCoords.z;
  float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
  float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
  return shadow;
}
```

---

## Shadow Map Improvement

- 빌드 및 결과
  - 거의 대부분의 영역에서 괜찮은 그림자를 렌더링

<div>
<img src="/opengl_course/note/images/13_shadow_bias_dynamic.png" width="60%"/>
</div>

---

## Peter Panning

- Peter panning 현상
  - bias 때문에 그림자가 물체와 떨어져서 그려지는 현상

<div>
<img src="/opengl_course/note/images/13_shadow_peter_panning.png" width="50%"/>
</div>

---

## Peter Panning

- Peter panning 현상 방지
  - shadow bias 값을 잘 조절한다
  - Shadow map을 그릴떄 face culling을 사용하여 뒷면만 그리도록 함

<div>
<img src="/opengl_course/note/images/13_shadow_peter_panning_solution.png" width="60%"/>
</div>

---

## Peter Panning

- bias를 0.001 ~ 0.02 사이로 조절한 결과

<div>
<img src="/opengl_course/note/images/13_shadow_peter_panning_after_bias.png" width="60%"/>
</div>

---

## Directional Light

- Directional light인 경우
  - light projection을 orthogonal로 변경

---

## Directional Light

- `shader/lighting_shadow.fs` 변경

```glsl [2, 7-24]
struct Light {
  int directional;
  vec3 position;
  // ...

// in main()
vec3 result = ambient;
vec3 lightDir;
float intensity = 1.0;
float attenuation = 1.0;
if (light.directional == 1) {
  lightDir = normalize(-light.direction);
}
else {
  float dist = length(light.position - fs_in.fragPos);
  vec3 distPoly = vec3(1.0, dist, dist*dist);
  attenuation = 1.0 / dot(distPoly, light.attenuation);
  lightDir = (light.position - fs_in.fragPos) / dist;

  float theta = dot(lightDir, normalize(-light.direction));
  intensity = clamp(
    (theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]),
    0.0, 1.0);
}
```

---

## Directional Light

- `Context`의 light parameter에 `directional` 멤버 추가

```cpp [3]
  // light parameter
  struct Light {
    bool directional { false };
```

---

## Directional Light

- `Context::Render()`에서 ImGui를 이용한 directional 값 조작 추가

```cpp
ImGui::Checkbox("l.directional", &m_light.directional);
```

---

## Directional Light

- directional light일 경우 light projection 을 orthogonal로 변경

```cpp [4-8]
auto lightView = glm::lookAt(m_light.position,
  m_light.position + m_light.direction,
  glm::vec3(0.0f, 1.0f, 0.0f));
auto lightProjection = m_light.directional ?
  glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 30.0f) :
  glm::perspective(
    glm::radians((m_light.cutoff[0] + m_light.cutoff[1]) * 2.0f),
    1.0f, 1.0f, 20.0f);
```

---

## Directional Light

- uniform parameter 설정 추가

```cpp [3-4]
m_lightingShadowProgram->Use();
m_lightingShadowProgram->SetUniform("viewPos", m_cameraPos);
m_lightingShadowProgram->SetUniform("light.directional",
  m_light.directional ? 1 : 0);
```

---

## Directional Light

- Directional light를 이용한 결과

<div>
<img src="/opengl_course/note/images/13_shadow_map_directional_light.png" width="60%"/>
</div>

---

## Oversampling

- 지면의 크기를 40 x 40으로 늘리고 far plane을 150으로 둔 결과
  - 비정상적인 그림자 발생

<div>
<img src="/opengl_course/note/images/13_shadow_map_oversampling.png" width="60%"/>
</div>

---

## Oversampling

- 발생 원인
  - 현재 texture의 wrapping 방식이 `GL_REPEAT`이기 때문
- 해결책
  - `GL_CLAMP_TO_BORDER`로 변경하고 border color를 지정

---

## Oversampling

- `Texture` 클래스에 border color 지정을 위한 함수 추가

```cpp
// texture.h
void SetBorderColor(const glm::vec4& color) const;
```

```cpp
// texture.cpp
void Texture::SetBorderColor(const glm::vec4& color) const {
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
    glm::value_ptr(color));
}
```

---

## Oversampling

- `ShadowMap` 초기화 시 texture 파라미터 변경

```cpp [3-4]
m_shadowMap = Texture::Create(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
m_shadowMap->SetFilter(GL_NEAREST, GL_NEAREST);
m_shadowMap->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
m_shadowMap->SetBorderColor(glm::vec4(1.0f));
```

---

## Oversampling

- 결과
  - 비정상적인 그림자가 제거됨

<div>
<img src="/opengl_course/note/images/13_shadow_map_oversampling_solved.png" width="60%"/>
</div>

---

## PCF

- Jagged edge
  - Depth map의 한정된 해상도에 의해서 딱딱한 블럭 모양의 그림자가 생성

<div>
<img src="/opengl_course/note/images/13_shadow_map_jagged_edge.png" width="60%"/>
</div>

---

## PCF

- Percentage Closer Filtering
  - shadow map으로부터 depth값을 여러 개 수집하자
  - 수집한 depth값들로 계산된 shadow 값의 평균을 구하자

---

## PCF

```glsl [2-11]
  float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);
  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
  for(int x = -1; x <= 1; ++x) {
      for (int y = -1; y <= 1; ++y) {
          float pcfDepth = texture(shadowMap,
            projCoords.xy + vec2(x, y) * texelSize).r;
          shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
      }
  }
  shadow /= 9.0;
```

---

## PCF

- 빌드 및 결과
  - 좀 더 부드러운 그림자

<div>
<img src="/opengl_course/note/images/13_shadow_map_pcf.png" width="60%"/>
</div>

---

## Shadow for Point Light

- Point light일 경우
  - 모든 방향으로 빛이 뻗어나감
  - 단일 shadow map은 하나의 방향만 고려할 수 밖에 없음

---

## Shadow for Point Light

- Omni-directional shadow map
  - 하나의 depth map에 그림을 그리는 대신
    depth cube map을 생성하자

<div>
<img src="/opengl_course/note/images/13_shadow_map_point_light_idea.png" width="60%"/>
</div>

---

## Normal Mapping

- Geometric detail
  - 해상도 좋은 텍스처를 입힌다 할 지라도 물체 표면의
    요철 모양은 vertex의 개수를 늘려야 제대로 표현 가능

<div>
<img src="/opengl_course/note/images/13_normal_map_example_before.png" width="40%"/>
</div>

---

## Normal Mapping

- Idea
  - 표면을 렌더링할 때 물체의 디테일을 결정하는 요소는 표면의 normal vector
  - normal vector에 대한 텍스처 맵을 만들어서 각 픽셀의 normal 방향을 변경시켜보자

<div>
<img src="/opengl_course/note/images/13_normal_map_idea.png" width="80%"/>
</div>

---

## Normal Mapping

- Normal map 적용 전후 비교
  - vertex를 늘리지 않고 높은 수준의 geometric detail 표현 가능

<div>
<img src="/opengl_course/note/images/13_normal_map_example_after.png" width="80%"/>
</div>

---

## Normal Mapping

- `image/brickwall.jpg` 저장

<div>
<img src="http://learnopengl.com/img/textures/brickwall.jpg" width="35%"/>
</div>

---

## Normal Mapping

- `image/brickwall_normal.jpg` 저장
  - 픽셀의 rgb 값에 normal xyz 좌표값이 저장되어 있음

<div>
<img src="http://learnopengl.com/img/textures/brickwall_normal.jpg" width="35%"/>
</div>

---

## Normal Mapping

- `shader/normal.vs` 구현

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 modelTransform;

out vec2 texCoord;
out vec3 position;

void main() {
  gl_Position = transform * vec4(aPos, 1.0);
  texCoord = aTexCoord;
  position = (modelTransform * vec4(aPos, 1.0)).xyz;
}
```

---

## Normal Mapping

- `shader/normal.fs` 구현

```glsl
#version 330 core

in vec2 texCoord;
in vec3 position;
out vec4 fragColor;

uniform vec3 viewPos;
uniform vec3 lightPos;

uniform sampler2D diffuse;
uniform sampler2D normalMap;

void main() {
  vec3 texColor = texture(diffuse, texCoord).xyz;
  vec3 pixelNorm = normalize(
    (texture(normalMap, texCoord).xyz * 2.0 - 1.0);
  vec3 ambient = texColor * 0.2;

  vec3 lightDir = normalize(lightPos - position);
  float diff = max(dot(pixelNorm, lightDir), 0.0);
  vec3 diffuse = diff * texColor * 0.8;

  vec3 viewDir = normalize(viewPos - position);
  vec3 reflectDir = reflect(-lightDir, pixelNorm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = spec * vec3(0.5);

  fragColor = vec4(ambient + diffuse + specular, 1.0);
}
```

---

## Normal Mapping

- `normal * 2.0 - 1.0`
  - texture 내의 픽셀값이 0~1 사이로 저장되어 있음
  - normal vector의 값의 범위는 -1~1

---

## Normal Mapping

- `Context` 클래스에 멤버 변수 추가

```cpp
  // normal map
  TextureUPtr m_brickDiffuseTexture;
  TextureUPtr m_brickNormalTexture;
  ProgramUPtr m_normalProgram;
```

---

## Normal Mapping

- `Context::Init()`에서 텍스처 및 쉐이더 초기화

```cpp
  m_brickDiffuseTexture = Texture::CreateFromImage(
    Image::Load("./image/brickwall.jpg", false).get());
  m_brickNormalTexture = Texture::CreateFromImage(
    Image::Load("./image/brickwall_normal.jpg", false).get());
  m_normalProgram = Program::Create(
    "./shader/normal.vs", "./shader/normal.fs");
```

---

## Normal Mapping

- `Context::Render()`에 렌더링 코드 추가

```cpp
  auto modelTransform =
    glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f));
  m_normalProgram->Use();
  m_normalProgram->SetUniform("viewPos", m_cameraPos);
  m_normalProgram->SetUniform("lightPos", m_light.position);
  glActiveTexture(GL_TEXTURE0);
  m_brickDiffuseTexture->Bind();
  m_normalProgram->SetUniform("diffuse", 0);
  glActiveTexture(GL_TEXTURE1);
  m_brickNormalTexture->Bind();
  m_normalProgram->SetUniform("normalMap", 1);
  glActiveTexture(GL_TEXTURE0);
  m_normalProgram->SetUniform("modelTransform", modelTransform);
  m_normalProgram->SetUniform("transform", projection * view * modelTransform);
  m_plane->Draw(m_normalProgram.get());
```

---

## Normal Mapping

- 빌드 및 실행
  - 빛의 위치를 변경해가면서 벽의 요철 모양이 잘 렌더링 되는지 확인

<div>
<img src="/opengl_course/note/images/13_normal_map_simple_result.png" width="60%"/>
</div>

---

## Normal Mapping

- 현재 구현의 문제점
  - 그리는 물체가 회전하면 렌더링이 제대로 되지 않음

<div>
<img src="/opengl_course/note/images/13_normal_map_simple_artifact.png" width="60%"/>
</div>

---

## Normal Mapping

- 원인
  - normal map으로부터 얻어낸 normal 값을 world space 기준으로 처리하고 있음
  - 물체의 local space를 기준으로 한다고 하더라도 normal map을 적용할 면이
    xy면에 정렬되어 있어야 함
  - 즉 평면이 아닌 입체에 대해서는 적용 불가능
- 해결책
  - Tangent space를 사용하자

---

## Tangent Space

- Tangent space
  - 면의 normal (법선) 방향과 tangent (접선) 방향이 만들어내는 공간
  - normal map이 보통 파란색인 이유
    - 대부분의 픽셀이 (0, 0, 1)에 근접한 normal 벡터를 갖기 때문

<div>
<img src="/opengl_course/note/images/13_normal_map_tangent_space.png" width="40%"/>
</div>

---

## Tangent Space

- Tangent space
  - TBN matrix
    - Tangent, Binormal, Normal으로 구성된 matrix
    - Tangent space를 local space로 변경하는 행렬로 사용
  - 주안점
    - Tangent space를 어떻게 구하는가?
    - 즉, Tangent 벡터를 어떻게 구하는가? 

---

## Tangent Space

- Tangent 벡터 구하는 방법
  - 텍스처 좌표를 이용

<div>
<img src="/opengl_course/note/images/13_normal_map_calculating_tangent.png" width="40%"/>
</div>

---

## Tangent Space

<div>
<img src="/opengl_course/note/images/13_normal_map_tangent_eq_01.png" height="8%"/>
<img src="/opengl_course/note/images/13_normal_map_tangent_eq_02.png" height="8%"/>
<img src="/opengl_course/note/images/13_normal_map_tangent_eq_03.png" height="8%"/>
<img src="/opengl_course/note/images/13_normal_map_tangent_eq_04.png" height="8%"/>
</div>

---

## Tangent Space

- `mesh.h`에서 `Vertex` 구조체에 `tangent` 멤버 추가
- `Mesh` 클래스에 `ComputeTangents()` 함수 추가

```cpp
  glm::vec3 tangent;
```

```cpp
  static void ComputeTangents(
    std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices);
```

---

## Tangent Space

- `Mesh::ComputeTangents()` 구현

```cpp
void Mesh::ComputeTangents(
  std::vector<Vertex>& vertices,
  const std::vector<uint32_t>& indices) {

  auto compute = [](
    const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3,
    const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3)
    -> glm::vec3 {

    auto edge1 = pos2 - pos1;
    auto edge2 = pos3 - pos1;
    auto deltaUV1 = uv2 - uv1;
    auto deltaUV2 = uv3 - uv1;
    float det = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    if (det != 0.0f) {
      auto invDet = 1.0f / det;
      return deltaUV2.y * edge1 - deltaUV1.y * edge2;
    }
    else {
      return glm::vec3(0.0f, 0.0f, 0.0f);
    }
  };

  // initialize
  std::vector<glm::vec3> tangents;
  tangents.resize(vertices.size());
  memset(tangents.data(), 0, tangents.size() * sizeof(glm::vec3));

  // accumulate triangle tangents to each vertex
  for (size_t i = 0; i < indices.size(); i += 3) {
    auto v1 = indices[i  ];
    auto v2 = indices[i+1];
    auto v3 = indices[i+2];

    tangents[v1] += compute(
      vertices[v1].position, vertices[v2].position, vertices[v3].position,
      vertices[v1].texCoord, vertices[v2].texCoord, vertices[v3].texCoord);

    tangents[v2] = compute(
      vertices[v2].position, vertices[v3].position, vertices[v1].position,
      vertices[v2].texCoord, vertices[v3].texCoord, vertices[v1].texCoord);

    tangents[v3] = compute(
      vertices[v3].position, vertices[v1].position, vertices[v2].position,
      vertices[v3].texCoord, vertices[v1].texCoord, vertices[v2].texCoord);
  }

  // normalize
  for (size_t i = 0; i < vertices.size(); i++) {
    vertices[i].tangent = glm::normalize(tangents[i]);
  }
}
```

---

## Tangent Space

- `Mesh::Init()`에서 `primitiveType`이 삼각형일 경우 tangent 계산

```cpp [5-7]
void Mesh::Init(
  const std::vector<Vertex>& vertices,
  const std::vector<uint32_t>& indices,
  uint32_t primitiveType) {
  if (primitiveType == GL_TRIANGLES) {
    ComputeTangents(const_cast<std::vector<Vertex>&>(vertices), indices);
  }
```

---

## Tangent Space

- `shader/normal.vs` 코드 수정

```glsl [4, 6, 13-14, 21-23]
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

uniform mat4 transform;
uniform mat4 modelTransform;

out vec2 texCoord;
out vec3 position;
out vec3 normal;
out vec3 tangent;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    texCoord = aTexCoord;
    position = (modelTransform * vec4(aPos, 1.0)).xyz;

    mat4 invTransModelTransform = transpose(inverse(modelTransform));
    normal = (invTransModelTransform * vec4(aNormal, 0.0)).xyz;
    tangent = (invTransModelTransform * vec4(aTangent, 0.0)).xyz;
}
```

---

## Tangent Space

- `shader/normal.fs` 코드 수정

```glsl [5-6, 12-18]
#version 330 core

in vec2 texCoord;
in vec3 position;
in vec3 normal;
in vec3 tangent;

//...

void main() {
    vec3 texColor = texture(diffuse, texCoord).xyz;
    vec3 texNorm = normalize(texture(normalMap, texCoord).xyz * 2.0 - 1.0);
    vec3 N = normalize(normal);
    vec3 T = normalize(tangent);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);
    vec3 pixelNorm = normalize(TBN * texNorm);

    vec3 ambient = texColor * 0.2;
```

---

## Tangent Space

- 빌드 및 결과
  - 임의의 회전에도 제대로 동작하는 normal shader

<div>
<img src="/opengl_course/note/images/13_normal_map_tangent_result.png" width="60%"/>
</div>

---

## Parallax Mapping

- Normal mapping의 단점
  - 큰 깊이감은 만들어 낼 수 없다
  - 경계면에서는 입체감을 가질 수 없다
- 해결책 1: Displacement mapping
  - vertex를 촘촘하게 생성하여 높낮이를 표현하는 방식
  - vertex를 많이 만들면 비용이 높다

---

## Parallax Mapping

- Height map과 Displacement mapping 결과

<div>
<img src="/opengl_course/note/images/13_displacement_map_height_map.png" width="30%"/>
<img src="/opengl_course/note/images/13_displacement_map_result.png" width="38%"/>
</div>

---

## Parallax Mapping

- Parallax mapping
  - view vector, height map이 주어졌을 때, normal mapping은 A 위치를 기준으로
    light를 계산한다
  - 실제 바라보는 위치 B는 어떻게 구할까?

<div>
<img src="/opengl_course/note/images/13_parallax_map_idea.png" width="50%"/>
</div>

---

## Parallax Mapping

- Parallax mapping

<div>
<img src="/opengl_course/note/images/13_parallax_map_result.png" width="55%"/>
</div>

---

## Parallax Mapping

- Normal mapping v.s. Parallax mapping

<div>
<img src="/opengl_course/note/images/13_parallax_map_comparison.png" width="70%"/>
</div>

---

## Congratulation!
### 수고하셨습니다!