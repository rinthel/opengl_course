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

## Shadow Mapping

- drawing shadow
  - shadow projection
  - shadow volume
  - shadow map
- depth map rendering
- shadow rendering
- improvement
  - shadow acne
  - peter panning (피터팬)
  - oversamping
- PCF (percentage-closer filtering)
- projection

---

## Shadow for Point Light

- depth cubemap rendering
- omni-directional shadow map
- PCF

---

## Normal Mapping

- idea
- normal map on plane
- non-plane and tangent space

---

## Parallax Mapping

- parallax mapping
- steep parallax mapping
- parallax occulsion mapping

---

## Congratulation!
### 수고하셨습니다!