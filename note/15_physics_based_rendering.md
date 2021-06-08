# Physics-Based Rendering

---

## Introduction

- 물리적인 방식을 근사하여 빛을 모사하는 렌더링
  - Microfacet model
  - 에너지 보존 법칙
  - BRDF
- Phong이나 Blinn 방식에 비해 좀 더 사실적
- 물리적인 파라미터로 재질 디자인이 가능

---

## Introduction

- 수업자료에서 다루는 PBR
  - Disney에서 연구, Epic Games에서 실시간 렌더링 방식에 차용
  - 현재 대부분의 게임 엔진에 구현되어 있음

<div>
<img src="/opengl_course/note/images/15_pbr_example.png" width="70%"/>
</div>

---

## Microfacet model

- 모든 면은 확대해서 보면 미세면(microfacet)이라 불리우는
  매우 작은 완전 반사 거울면으로 이루어져 있다
  - 거친면 => 반사광 분산 => 넓은 하이라이트
  - 부드러운면 => 반사광 균일 => 작은 하이라이트

<div>
<img src="/opengl_course/note/images/15_pbr_microfacets.png" width="50%"/>
</div>

---

## Microfacet model

- 미세면은 너무 작아서 픽셀 레벨에서 구분 불가
- **roughness** 라는 파라미터로 면의 거친 정도를
  통계적으로 근사함
  - halfway 벡터에 정렬된 microfacets 비율

<div>
<img src="/opengl_course/note/images/15_pbr_roughness.png" width="60%"/>
</div>

---

## Energy conservation

- 출력되는 빛의 에너지는 입력되는 빛의 에너지를 초과할 수 없다
  - 하이라이트 면적이 커지면 밝기가 줄어드는 이유
- 입사광이 물체에 닿으면 굴절광과 반사광으로 나뉘어 에너지 보존을 성립시킨다
  - 반사 (reflection) => 반사광 (specular)
  - 굴절 (refraction) => 분산광 (diffuse)

---

## Energy conservation

- 굴절된 빛은 재질 표면의 입자들에 부딪힘
  - 흡수되어 열 에너지로 전환
  - 임의의 방향으로 출력

<div>
<img src="/opengl_course/note/images/15_pbr_energy_conservation.png" width="50%"/>
</div>

---

## Energy conservation

- Dielectric (유전체)
  - 입사한 빛 중 일부가 재질 표면 입자에 부딪히다 출력
  - Subsurface scattering
- Metallic
  - 굴절광은 모두 흡수되어 분산광 색상이 사라짐

---

## Energy conservation

- 에너지 보존 법칙을 고려한 reflection / refraction의 관계식

```glsl
// reflection/specular fraction
float kS = calculateSpecularComponent(...);
// refraction/diffuse fraction
float kD = 1.0 - kS; 
```

---

## Reflectance equation

- 반사율 방정식
- Rendering equation의 특수한 형태

<div>
<img src="/opengl_course/note/images/15_pbr_reflectance_equation.png" width="70%"/>
</div>

---

## Reflectance equation

- Radiant flux **Φ**: light source로부터 전송된 에너지 (단위: Watts)
  - 파장 길이에 대한 에너지 함수를 적분한 값
  - RGB값으로 단순화하여 사용

<div>
<img src="/opengl_course/note/images/15_pbr_daylight_spectrum.png" width="40%"/>
</div>

---

## Reflectance equation

- Solid angle **ω**: 단위 구에 투영한 면적과 각도를 가진 쉐입
- Radiant intensity **I**: 단위 구체 상의 면적당 광원의 세기
  - **I** = d**Φ** / d**ω**

<div>
<img src="/opengl_course/note/images/15_pbr_solid_angle.png" width="30%"/>
<img src="/opengl_course/note/images/15_pbr_radiant_intensity.png" width="30%"/>
</div>

---

## Reflectance equation

- Radiance **L**: 광원으로부터 **Φ**만큼의 에너지가 **θ**의 각도로
  입사했을 때 단위 면적 당 방출된 전자기파의 양

<div>
<img src="/opengl_course/note/images/15_pbr_radiance.png" width="40%"/>
</div>

---

## Reflectance equation

- Reflected radiance **L**o
  - 특정 위치 p에 출사 방향 벡터 **ω**o로 나오는 빛의 에너지 크기
  - 반구 **Ω**로 들어오는 모든 영역에서 입사 방향 벡터 **ω**i에 대한
    빛 에너지의 가중치 합

<div>
<img src="/opengl_course/note/images/15_pbr_reflectance_equation.png" width="70%"/>
</div>

---

## Reflectance equation

- Reflectance equation implementation

```glsl
int steps = 100;
float sum = 0.0f;
vec3 P = ...;
vec3 Wo = ...;
vec3 N = ...;
float dW = 1.0f / steps;
for(int i = 0; i < steps; ++i) {
  vec3 Wi = getNextIncomingLightDir(i);
  sum += Fr(P, Wi, Wo) * L(P, Wi) * dot(N, Wi) * dW;
}
```

---

## BRDF

- BRDF
  - Bidirectional Reflective Distribution Function
    - 양방향 반사도 분포 함수
  - 입력: 입사각, 출사각, 법선벡터, roughness
  - 특정 입사각으로 들어오는 광선이 반사광에 얼마나 기여하는가에 대한 분포 함수
  - 거울의 BRDF 함수
    - **ω**o와 **ω**i가 서로 반사벡터 관계일 때만 1, 그외에는 0

---

## BRDF

- Cook-Torrance BRDF
  - 대부분의 실시간 PBR 렌더 파이프라인이 사용하는 BRDF
  - diffuse / specular term으로 나뉘어져 있음

<div>
<img src="/opengl_course/note/images/15_pbr_cook_torrance_brdf.png" width="70%"/>
</div>

---

## BRDF

- Refraction (diffuse) term
  - Lambertian diffuse
    - c: albedo (surface color)

<div>
<img src="/opengl_course/note/images/15_pbr_brdf_lambertian_diffuse.png" width="25%"/>
</div>

---

## BRDF

- Reflection (specular) term
  - D: normal distribution function
  - G: geometry function
  - F: Fresnel equation
  - 다양한 approximation function이 있음
  - 이 수업자료의 function: Unreal Engine 4에서 채택한 함수들

<div>
<img src="/opengl_course/note/images/15_pbr_brdf_reflection.png" width="50%"/>
</div>

---

## BRDF

- D: Trowbridge-Reitz GGX
  - 미세면이 halfway 벡터에 얼마나 정렬되어 있는지를 나타내는 분포함수
    - **n**: normal vector
    - **h**: halfway vector
    - **α**: roughness

<div>
<img src="/opengl_course/note/images/15_pbr_trowbridge_reitz_ggx.png" width="50%"/>
</div>

---

## BRDF

- D: Trowbridge-Reitz GGX

<div>
<img src="/opengl_course/note/images/15_pbr_roughness.png" width="80%"/>
</div>

```glsl
float DistributionGGX(vec3 N, vec3 H, float a) {
  float a2 = a*a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH*NdotH;

  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return a2 / denom;
}
```

---

## BRDF

- G: Schlick-GGX
  - 미세면의 거친 정도에 따라 발생하는 self-shadow의 정도
  - 빛의 방향 / 시선의 방향과도 연관이 있음

<div>
<img src="/opengl_course/note/images/15_pbr_geometry_function.png" width="50%"/>
</div>

---

## BRDF

- G: Schlick-GGX
  - **n**: normal
  - **v**: view
  - **k**
    - Direct light: (**α** + 1)^2 / 8
    - Image-based light: **α**^2 / 2

<div>
<img src="/opengl_course/note/images/15_pbr_schlick_ggx.png" width="50%"/>
</div>

---

## BRDF

- G: Schlick-GGX
  - Smith's method
    - 빛의 방향 및 시선 방향 모두를 고려한 geometry function


<div>
<img src="/opengl_course/note/images/15_pbr_geometry_smith.png" width="50%"/>
</div>
<div>
<img src="/opengl_course/note/images/15_pbr_roughness_geometry.png" width="70%"/>
</div>

---

## BRDF

- G: Schlick-GGX

```glsl
float GeometrySchlickGGX(float NdotV, float k) {
  float nom = NdotV;
  float denom = NdotV * (1.0 - k) + k;
  return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float k) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx1 = GeometrySchlickGGX(NdotV, k);
  float ggx2 = GeometrySchlickGGX(NdotL, k);
  return ggx1 * ggx2;
}
```

---

## BRDF

- F: Fresnel equation
  - 시점이 주어졌을 때 표면에서 반사된 빛과 굴절된 빛 간의 비율
    - 모든 면은 수직하게 관측하면 빛을 모두 반사시킨다
  - Fresnel-Schlick approximation

<div>
<img src="/opengl_course/note/images/15_pbr_fresnel_schlick.png" width="60%"/>
</div>

---

## BRDF

- F: Fresnel equation
  - F0: 면의 기본 반사율
    - 재질에 따라 계산된 측정값 참고

<div>
<img src="/opengl_course/note/images/15_pbr_fresnel_render.png" width="20%"/>
</div>

---

## BRDF

- F0 값 테이블

<div>
<img src="/opengl_course/note/images/15_pbr_base_reflectivity_table.png" width="100%"/>
</div>

---

## BRDF

- Metallic workflow
  - Metallic: 금속과 비금속의 정도를 0~1사이로 표현한 값
    - 평균적인 비금속 물질의 F0값 0.04와 linear interpolation한 값을
      공식에 적용

```glsl
vec3 fresnelSchlick(float cosTheta, vec3 surfaceColor, float metallic) {
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, surfaceColor.rgb, metallic);
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
```

---

## BRDF

- Cook-Torrance reflectance equation
  - Cook-Torrance BRDF가 적용된 반사율 방정식
  - Fresnel 값이 **k**s를 포함하고 있어 식에서 제외

<div>
<img src="/opengl_course/note/images/15_pbr_cook_torrance_reflectance_eq.png" width="80%"/>
</div>

---

## Authoring PBR materials

- PBR 재질의 파라미터
  - Albedo: 면의 기본색상 (금속인 경우 F0값)
  - Normal: 법선 방향
  - Metallic: 금속성 / 비금속성
  - Roughness: 면의 거친정도
  - AO: Ambient Occlusion

---

## Authoring PBR materials

- PBR 재질의 예

<div>
<img src="/opengl_course/note/images/15_pbr_material.png" width="80%"/>
</div>

---

## Lighting

- PBR 이론을 바탕으로 shading 해보기
  - 점 광원이 하나 있는 경우
    - **L**: light color
    - 광원의 입사 벡터 **w**i
    - 광원과 **p** 간의 거리에 의한 attenuation 및
      dot(n, wi)으로 인한 조정

<div>
<img src="/opengl_course/note/images/15_pbr_lighting_point_light.png" width="40%"/>
</div>

---

## Lighting

- PBR 이론을 바탕으로 shading 해보기
  - 결국 점 광원에 대한 radiance 계산은 Phong의 diffuse 계산과 매우 유사하다
  - 광원이 여러개 있는 경우
    - 모든 radiance의 합산

---

## Lighting

- 예제 준비
  - `advanced_lighting_example` 프로젝트를 복사하여 `pbr_example` 프로젝트 구성

---

## Lighting

- Context 코드 간소화

```cpp
CLASS_PTR(Context)
class Context {
public:
  static ContextUPtr Create();
  void Render();
  void ProcessInput(GLFWwindow* window);
  void Reshape(int width, int height);
  void MouseMove(double x, double y);
  void MouseButton(int button, int action, double x, double y);

  void DrawScene(const glm::mat4& view,
    const glm::mat4& projection,
    const Program* program);

private:
  Context() {}
  bool Init();
  
  ProgramUPtr m_simpleProgram;

  MeshUPtr m_box;
  MeshUPtr m_plane;

  // screen size
  int m_width {640};
  int m_height {480};

  // camera parameter
  bool m_cameraControl { false };
  glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
  float m_cameraPitch { 0.0f };
  float m_cameraYaw { 0.0f };
  glm::vec3 m_cameraFront { glm::vec3(0.0f, -1.0f, 0.0f) };
  glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f, 8.0f) };
  glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };
};
```

---

## Lighting

- Context 코드 간소화

```cpp
void Context::Reshape(int width, int height) {
  m_width = width;
  m_height = height;
  glViewport(0, 0, m_width, m_height);
}
```

---

## Lighting

- Context 코드 간소화

```cpp
bool Context::Init() {
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  m_box = Mesh::CreateBox();
  m_plane = Mesh::CreatePlane();

  m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
  return true;
}
```

---

## Lighting

- Context 코드 간소화

```cpp
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
  m_simpleProgram->Use();
  m_simpleProgram->SetUniform("color", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
  m_simpleProgram->SetUniform("transform", projection * view);
  m_box->Draw(m_simpleProgram.get());
}
```

---

## Lighting

- Context 코드 간소화

```cpp
void Context::DrawScene(const glm::mat4& view,
  const glm::mat4& projection,
  const Program* program) {
  
}
```

---

## Lighting

- 빌드 및 실행

<div>
<img src="/opengl_course/note/images/15_pbr_example_setup.png" width="80%"/>
</div>

---

## Lighting

- `Mesh` 클래스에 구 생성 함수 추가

```cpp
// mesh.h
  static MeshUPtr CreateSphere(
      uint32_t latiSegmentCount = 16,
      uint32_t longiSegmentCount = 32);
```

---

## Lighting

- `Mesh::CreateSphere()` 구현

```cpp
MeshUPtr Mesh::CreateSphere(
  uint32_t latiSegmentCount,
  uint32_t longiSegmentCount) {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;

  uint32_t circleVertCount = longiSegmentCount + 1;
  vertices.resize((latiSegmentCount + 1) * circleVertCount);
  for (uint32_t i = 0; i <= latiSegmentCount; i++) {
    float v = (float)i / (float)latiSegmentCount;
    float phi = (v - 0.5f) * glm::pi<float>();
    auto cosPhi = cosf(phi);
    auto sinPhi = sinf(phi);
    for (uint32_t j = 0; j <= longiSegmentCount; j++) {
      float u = (float)j / (float)longiSegmentCount;
      float theta = u * glm::pi<float>() * 2.0f;
      auto cosTheta = cosf(theta);
      auto sinTheta = sinf(theta);
      auto point = glm::vec3(
        cosPhi * cosTheta, sinPhi, -cosPhi * sinTheta);
      
      vertices[i * circleVertCount + j] = Vertex {
        point * 0.5f, point, glm::vec2(u, v), glm::vec3(0.0f)
      };
    }
  }

  indices.resize(latiSegmentCount * longiSegmentCount * 6);
  for (uint32_t i = 0; i < latiSegmentCount; i++) {
    for (uint32_t j = 0; j < longiSegmentCount; j++) {
      uint32_t vertexOffset = i * circleVertCount + j;
      uint32_t indexOffset = (i * longiSegmentCount + j) * 6;
      indices[indexOffset    ] = vertexOffset;
      indices[indexOffset + 1] = vertexOffset + 1;
      indices[indexOffset + 2] = vertexOffset + 1 + circleVertCount;
      indices[indexOffset + 3] = vertexOffset;
      indices[indexOffset + 4] = vertexOffset + 1 + circleVertCount;
      indices[indexOffset + 5] = vertexOffset + circleVertCount;
    }
  }

  return Create(vertices, indices, GL_TRIANGLES);
}
```

---

## Lighting

- `Context` 클래스에 구 메쉬 멤버 추가

```cpp [4]
// context.h
  MeshUPtr m_box;
  MeshUPtr m_plane;
  MeshUPtr m_sphere;
```

---

## Lighting

- `Context::Init()` 함수에서 구 생성

```cpp [3]
  m_box = Mesh::CreateBox();
  m_plane = Mesh::CreatePlane();
  m_sphere = Mesh::CreateSphere();
```

---

## Lighting

- `Context::DrawScene()`에서 배열 형태로 구 그리기
  - roughness / metallic 관측용

```cpp [5-20]
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
      m_sphere->Draw(program);
    }
  }
}
```

---

## Lighting

- `Context::Render()`에서 `Context::DrawScene()` 호출

```cpp [5]
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_simpleProgram->Use();
  m_simpleProgram->SetUniform("color",
    glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
  DrawScene(view, projection, m_simpleProgram.get());
```

---

## Lighting

- 빌드 및 결과

<div>
<img src="/opengl_course/note/images/15_pbr_example_setup_2.png" width="80%"/>
</div>

---

## Lighting

- `shader/pbr.vs` 추가
  - `shader/lighting.vs`과 거의 동일

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 modelTransform;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

void main() {
  gl_Position = transform * vec4(aPos, 1.0);
  fragPos = (modelTransform * vec4(aPos, 1.0)).xyz;
  normal = (transpose(inverse(modelTransform)) * vec4(aNormal, 0.0)).xyz;
  texCoord = aTexCoord;
}
```

---

## Lighting

- `shader/pbr.fs` 추가

```glsl
#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 fragPos;

out vec4 fragColor;

uniform vec3 viewPos;

struct Light {
  vec3 position;
  vec3 color;
};
const int LIGHT_COUNT = 4;
uniform Light lights[LIGHT_COUNT];

struct Material {
  vec3 albedo;
  float metallic;
  float roughness;
  float ao;
};
uniform Material material;

const float PI = 3.14159265359;

float DistributionGGX(vec3 normal, vec3 halfDir, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float dotNH = max(dot(normal, halfDir), 0.0);
  float dotNH2 = dotNH * dotNH;

  float num = a2;
  float denom = (dotNH2 * (a2 - 1.0) + 1.0);
  return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float dotNV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r*r) / 8.0;

  float num = dotNV;
  float denom = dotNV * (1.0 - k) + k;
  return num / denom;
}

float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) {
  float dotNV = max(dot(normal, viewDir), 0.0);
  float dotNL = max(dot(normal, lightDir), 0.0);
  float ggx2 = GeometrySchlickGGX(dotNV, roughness);
  float ggx1 = GeometrySchlickGGX(dotNL, roughness);
  return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
  vec3 albedo = material.albedo;
  float metallic = material.metallic;
  float roughness = material.roughness;
  float ao = material.ao;
  vec3 fragNormal = normalize(normal);
  vec3 viewDir = normalize(viewPos - fragPos);
  float dotNV = max(dot(fragNormal, viewDir), 0.0);

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);

  // reflectance equation
  vec3 outRadiance = vec3(0.0);
  for (int i = 0; i < LIGHT_COUNT; i++) {
    // calculate per-light radiance
    vec3 lightDir = normalize(lights[i].position - fragPos);
    vec3 halfDir = normalize(viewDir + lightDir);

    float dist = length(lights[i].position - fragPos);
    float attenuation = 1.0 / (dist * dist);
    vec3 radiance = lights[i].color * attenuation;

    // Cook-Torrance BRDF
    float ndf = DistributionGGX(fragNormal, halfDir, roughness);
    float geometry = GeometrySmith(fragNormal, viewDir, lightDir, roughness);
    vec3 fresnel = FresnelSchlick(max(dot(halfDir, viewDir), 0.0), F0);

    vec3 kS = fresnel;
    vec3 kD = 1.0 - kS;
    kD *= (1.0 - metallic);

    float dotNL = max(dot(fragNormal, lightDir), 0.0);
    vec3 numerator = ndf * geometry * fresnel;
    float denominator = 4.0 * dotNV * dotNL;
    vec3 specular = numerator / max(denominator, 0.001);

    // add to outgoing radiance Lo
    outRadiance += (kD * albedo / PI + specular) * radiance * dotNL;
  }

  vec3 ambient = vec3(0.03) * albedo * ao;
  vec3 color = ambient + outRadiance;

  // Reinhard tone mapping + gamma correction
  color = color / (color + 1.0);
  color = pow(color, vec3(1.0 / 2.2));

  fragColor = vec4(color, 1.0);
}
```

---

## Lighting

- `Context`에 PBR 프로그램, light, material 멤버 추가

```cpp [2, 8-20]
  ProgramUPtr m_simpleProgram;
  ProgramUPtr m_pbrProgram;

  MeshUPtr m_box;
  MeshUPtr m_plane;
  MeshUPtr m_sphere;

  struct Light {
    glm::vec3 position { glm::vec3(0.0f, 0.0f, 0.0f) };
    glm::vec3 color { glm::vec3(1.0f, 1.0f, 1.0f) };
  };
  std::vector<Light> m_lights;

  struct Material {
    glm::vec3 albedo { glm::vec3(1.0f, 1.0f, 1.0f) };
    float roughness { 0.5f };
    float metallic { 0.5f };
    float ao { 0.1f };
  };
  Material m_material;
```

---

## Lighting

- `Context::Init()`에서 PBR 프로그램 초기화 및 light 초기화

```cpp [2-15]
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
```

---

## Lighting

- `Context::Render()`에서 렌더링 코드 수정

```cpp [2-11]
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
```

---

## Lighting

- `Context::Render()`의 ImGui 코드에 light 및 material 조작 UI 추가

```cpp
  // in ImGui::Begin()
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
```

---

## Lighting

- `Context::DrawScene()`에서 각 sphere 렌더링 전
  각기 다른 roughness, metallic 파라미터 세팅

```cpp [3-6]
  program->SetUniform("transform", transform);
  program->SetUniform("modelTransform", modelTransform);
  program->SetUniform("material.roughness",
    (float)(i + 1) / (float)sphereCount);
  program->SetUniform("material.metallic",
    (float)(j + 1) / (float)sphereCount);
  m_sphere->Draw(program);
```

---

## Lighting

- 빌드 및 결과
  - 빛의 위치 및 색상, 카메라 위치 등을 달리하며 결과 관찰

<div>
<img src="/opengl_course/note/images/15_pbr_example_first_result.png" width="67%"/>
</div>

---

## Textured PBR

- [freepbr.com](https://freepbr.com/materials/rusted-iron-pbr-metal-material-alt/)에서 
  `rustediron1-alt2-bl.zip` 다운로드
  - png image 4장
  - `image` 디렉토리에 압축 해제

<div>
<img src="https://freepbr.com/wp-content/uploads/2016/06/iron-rusted-preview2b.png" width="30%"/>
</div>

---

## Textured PBR

- `shader/pbr_texture.vs` 작성
  - `shader/pbr.vs` + normal mapping

```cpp
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

uniform mat4 transform;
uniform mat4 modelTransform;

out vec3 fragPos;
out vec2 texCoord;
out mat3 TBN;

void main() {
  gl_Position = transform * vec4(aPos, 1.0);
  fragPos = (modelTransform * vec4(aPos, 1.0)).xyz;
  texCoord = aTexCoord;

  mat4 invTransModelTransform = transpose(inverse(modelTransform));
  vec3 normal = normalize((invTransModelTransform * vec4(aNormal, 0.0)).xyz);
  vec3 tangent = normalize((invTransModelTransform * vec4(aTangent, 0.0)).xyz);
  vec3 binormal = cross(normal, tangent);
  TBN = mat3(tangent, binormal, normal);
}
```

---

## Textured PBR

- `shader/pbr_texture.fs` 작성
  - `shader/pbr.fs` 복사 후 수정

```glsl [4, 9-12, 19-24]
#version 330 core
in vec3 fragPos;
in vec2 texCoord;
in mat3 TBN;

// ...

struct Material {
  sampler2D albedo;
  sampler2D metallic;
  sampler2D roughness;
  sampler2D normal;
  float ao;
};

// ...

void main() {
  vec3 albedo = pow(texture(material.albedo, texCoord).rgb, vec3(2.2));
  float metallic = texture(material.metallic, texCoord).r;
  float roughness = texture(material.roughness, texCoord).r;
  float ao = material.ao;
  vec3 fragNormal = texture(material.normal, texCoord).rgb * 2.0 - 1.0;
  fragNormal = TBN * fragNormal;
  vec3 viewDir = normalize(viewPos - fragPos);
```

---

## Textured PBR

- albedo 값에 2.2승을 취하는 이유
  - sRGB -> linear RGB space 전환

<div>
<img src="https://www.mathworks.com/matlabcentral/mlc-downloads/downloads/d1d5b469-89ab-47c9-aa17-d6fefaf684ac/802dd546-e1e3-4b43-b347-d3248317e516/images/screenshot.jpg" width="50%"/>
</div>

---

## Textured PBR

- `Context` 클래스의 `Material` 멤버 변경

```cpp [2-5]
  struct Material {
    TexturePtr albedo;
    TexturePtr roughness;
    TexturePtr metallic;
    TexturePtr normal;
    float ao { 0.1f };
  };
```

---

## Textured PBR

- `Context::Init()`에서 PBR 프로그램 변경 및 텍스처 로딩

```cpp [3-13]
  m_simpleProgram = Program::Create(
    "./shader/simple.vs", "./shader/simple.fs");
  m_pbrProgram = Program::Create(
    "./shader/pbr_texture.vs", "./shader/pbr_texture.fs");

  m_material.albedo = Texture::CreateFromImage(
    Image::Load("./image/rustediron2_basecolor.png").get());
  m_material.roughness = Texture::CreateFromImage(
    Image::Load("./image/rustediron2_roughness.png").get());
  m_material.metallic = Texture::CreateFromImage(
    Image::Load("./image/rustediron2_metallic.png").get());
  m_material.normal = Texture::CreateFromImage(
    Image::Load("./image/rustediron2_normal.png").get());
```

---

## Textured PBR

- `Context::Render()`에서 재질값 변경을 위한 UI 제거 / 렌더링 코드 변경

```cpp [4-16]
  m_pbrProgram->Use();
  m_pbrProgram->SetUniform("viewPos", m_cameraPos);
  m_pbrProgram->SetUniform("material.ao", m_material.ao);
  m_pbrProgram->SetUniform("material.albedo", 0);
  m_pbrProgram->SetUniform("material.roughness", 1);
  m_pbrProgram->SetUniform("material.metallic", 2);
  m_pbrProgram->SetUniform("material.normal", 3);
  glActiveTexture(GL_TEXTURE0);
  m_material.albedo->Bind();
  glActiveTexture(GL_TEXTURE1);
  m_material.roughness->Bind();
  glActiveTexture(GL_TEXTURE2);
  m_material.metallic->Bind();
  glActiveTexture(GL_TEXTURE3);
  m_material.normal->Bind();
  glActiveTexture(GL_TEXTURE0);
```

---

## Textured PBR

- `Context::DrawScene()`에서 불필요한 uniform 설정 코드 제거

```cpp
  float x = ((float)i - (float)(sphereCount - 1) * 0.5f) * offset;
  auto modelTransform =
      glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
  auto transform = projection * view * modelTransform;
  program->SetUniform("transform", transform);
  program->SetUniform("modelTransform", modelTransform);
  m_sphere->Draw(program);
```

---

## Textured PBR

- 빌드 및 결과

<div>
<img src="/opengl_course/note/images/15_pbr_example_texture_result.png" width="80%"/>
</div>

---

## IBL

- Image Based Lighting
  - environment map을 하나의 큰 광원으로 사용하는 기술
  - cubemap의 각 픽셀이 광원이 된다
  - 장면이 환경 안에 있는 느낌을 더 잘 표현할 수 있다 

---

## IBL

- IBL 적용하기
  - direct lighting의 경우 각 광원의 기여도의 합
  - IBL의 경우 모든 방향에서의 광원을 고려해야함

<div>
<img src="/opengl_course/note/images/15_pbr_cook_torrance_reflectance_eq.png" width="80%"/>
</div>

---

## IBL

- IBL 적용하기
  - 적분식 계산을 미리 해둬서 real-time에 동작하도록 하자
  - 적분 기호의 분배

<div>
<img src="/opengl_course/note/images/15_pbr_cook_torrance_refeq_divide.png" width="90%"/>
</div>

---

## IBL

- Diffuse irradiance
  - Lambert term은 상수이므로 적분식 밖으로 빼낼 수 있음
  - **p**가 환경맵의 한가운데 있다고 가정할 경우 적분식은 **W**i
    에 대한 식으로 볼 수 있음

<div>
<img src="/opengl_course/note/images/15_pbr_diffuse_irradiance_eq.png" width="50%"/>
</div>

---

## Diffuse Irradiance Map

- 환경맵을 Convolution하여 diffuse irradiance를 미리 계산해둘 수 있음
  - 출력 방향 **w**o에 대하여 반구 내 모든 입력 방향 **w**i에 대한 빛의
    가중치 합산

<div>
<img src="/opengl_course/note/images/15_pbr_diffuse_irradiance_figure.png" width="40%"/>
</div>

---

## Diffuse Irradiance Map

- Wave 엔진에서 계산 한 환경 맵과 irradiance map의 예

<div>
<img src="/opengl_course/note/images/15_pbr_irradiance_map_example.png" width="80%"/>
</div>

---

## Radiance HDR file

- 일반적인 환경 맵으로 계산된 irradiance map은 제대로 동작하지 않음
  - 값의 범위가 0 ~ 1 사이로 잘려서 들어가기 때문
  - 1 이상의 값을 저장할 수 있는 HDR 포맷의 환경 맵을 사용해야 함

---

## Radiance HDR file

- [hdrlabs.com](http://www.hdrlabs.com/sibl/archive.html)에서
  **Alexs Apartment** 다운로드
  - `Alexs_Apt_2k.hdr`을 `image` 디렉토리에 저장
  - cubemap이 아닌 equirectangular map 형태로 저장되어 있음

<div>
<img src="/opengl_course/note/images/15_pbr_hdr_map_example.jpg" width="60%"/>
</div>

---

## Radiance HDR file

- HDR 환경 맵을 실제로 사용하기 위해 해결해야 할 문제들
  - `.hdr` 파일 읽기
    - floating point image
  - equirectangular => cubemap 전환하기
    - cubemap 형태의 framebuffer 생성
    - equirectangular image를 cubemap에 렌더링

---

## Equirectangular-to-Cube

- `Image` 클래스 메소드 리팩토링
  - 채널 당 바이트 수 저장
  - 입력된 이미지 확장자가 hdr일 경우 float 데이터로 읽기

---

## Equirectangular-to-Cube

- `Image` 클래스 메소드 리팩토링

```cpp [6-7, 16, 23-24, 28]
CLASS_PTR(Image)
class Image {
public:
  static ImageUPtr Load(const std::string& filepath,
    bool flipVertical = true);
  static ImageUPtr Create(int width, int height,
    int channelCount = 4, int bytePerChannel = 1);
  static ImageUPtr CreateSingleColorImage(int width, int height,
    const glm::vec4& color);
  ~Image();

  const uint8_t* GetData() const { return m_data; }
  int GetWidth() const { return m_width; }
  int GetHeight() const { return m_height; }
  int GetChannelCount() const { return m_channelCount; }
  int GetBytePerChannel() const { return m_bytePerChannel; }

  void SetCheckImage(int gridX, int gridY);
    
private:
  Image() {};
  bool LoadWithStb(const std::string& filepath, bool flipVertical);
  bool Allocate(int width, int height,
    int channelCount, int bytePerChannel);
  int m_width { 0 };
  int m_height { 0 };
  int m_channelCount { 0 };
  int m_bytePerChannel { 1 };
  uint8_t* m_data { nullptr };
};
```

---

## Equirectangular-to-Cube

- `Image` 클래스 메소드 리팩토링

```cpp [1-2, 4]
ImageUPtr Image::Create(int width, int height,
  int channelCount, int bytePerChannel) {
  auto image = ImageUPtr(new Image());
  if (!image->Allocate(width, height, channelCount, bytePerChannel))
    return nullptr;
  return std::move(image);
}
```

---

## Equirectangular-to-Cube

- `Image` 클래스 메소드 리팩토링

```cpp [1-2, 6-8]
bool Image::Allocate(int width, int height,
  int channelCount, int bytePerChannel) {
  m_width = width;
  m_height = height;
  m_channelCount = channelCount;
  m_bytePerChannel = bytePerChannel;
  m_data = (uint8_t*)malloc(m_width * m_height *
    m_channelCount * m_bytePerChannel);
  return m_data ? true : false;
}
```

---

## Equirectangular-to-Cube

- `Image` 클래스 메소드 리팩토링

```cpp [4-14]
bool Image::LoadWithStb(const std::string& filepath,
  bool flipVertical) {
  stbi_set_flip_vertically_on_load(flipVertical);
  auto ext = filepath.substr(filepath.find_last_of('.'));
  if (ext == ".hdr" || ext == ".HDR") {
    m_data = (uint8_t*)stbi_loadf(filepath.c_str(),
      &m_width, &m_height, &m_channelCount, 0);
    m_bytePerChannel = 4;
  }
  else {
    m_data = stbi_load(filepath.c_str(),
      &m_width, &m_height, &m_channelCount, 0);
    m_bytePerChannel = 1;
  }
  if (!m_data) {
    SPDLOG_ERROR("failed to load image: {}", filepath);
    return false;
  }
  return true;
}
```

---

## Equirectangular-to-Cube

- `Texture` 클래스 메소드 리팩토링
  - `Image` 인스턴스로부터 텍스처 생성시 `bytePerChannel` 값을 참조
  - `bytePerChannel`이 4일 경우 `GL_RGB` 대신 `GL_RGB16F`를 사용

---

## Equirectangular-to-Cube

- `Texture` 클래스 메소드 리팩토링

```cpp [14-23]
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
  if (image->GetBytePerChannel() == 4) {
    m_type = GL_FLOAT;
    switch (image->GetChannelCount()) {
      default: break;
      case 1: m_format = GL_R16F; break;
      case 2: m_format = GL_RG16F; break;
      case 3: m_format = GL_RGB16F; break;
      case 4: m_format = GL_RGBA16F; break;
    }
  }
  
  glTexImage2D(GL_TEXTURE_2D, 0, m_format,
    m_width, m_height, 0,
    format, m_type,
    image->GetData());

  glGenerateMipmap(GL_TEXTURE_2D);
}
```

---

## Equirectangular-to-Cube

- `shader/spherical_map.vs` 추가
  - `shader/skybox.vs`와 거의 동일

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 localPos;
uniform mat4 transform;

void main() {
  gl_Position = transform * vec4(aPos, 1.0);
  localPos = aPos;
}
```

---

## Equirectangular-to-Cube

- `shader/spherical_map.fs` 추가

```glsl
#version 330 core
out vec4 fragColor;
in vec3 localPos;
uniform sampler2D tex;

const vec2 invPi = vec2(0.1591549, 0.3183098862);
vec2 SampleSphericalMap(vec3 v) {
  return vec2(atan(v.z, v.x), asin(v.y)) * invPi + 0.5;
}

void main() {
  vec2 uv = SampleSphericalMap(normalize(localPos)); // normalize
  vec3 color = texture(tex, uv).rgb;
  fragColor = vec4(color, 1.0);
}
```

---

## Equirectangular-to-Cube

- `Context` 클래스에 HDR equirectangular map을 cube로 그리기 위한 멤버 추가

```cpp
  // context.h
  TextureUPtr m_hdrMap;
  ProgramUPtr m_sphericalMapProgram;
```

---

## Equirectangular-to-Cube

- `Context::Init()`에서 텍스처 및 프로그램 초기화

```cpp
  m_hdrMap = Texture::CreateFromImage(
    Image::Load("./image/Alexs_Apt_2k.hdr").get());
  m_sphericalMapProgram = Program::Create(
    "./shader/spherical_map.vs", "./shader/spherical_map.fs");
```

---

## Equirectangular-to-Cube

- `Context::Render()`에서 렌더링 코드 호출

```cpp
  m_sphericalMapProgram->Use();
  m_sphericalMapProgram->SetUniform("transform",
    projection * view *
    glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 2.0f)));
  m_sphericalMapProgram->SetUniform("tex", 0);
  m_hdrMap->Bind();
  m_box->Draw(m_sphericalMapProgram.get());
```

---

## Equirectangular-to-Cube

- 빌드 및 결과
  - exposure tone mapping을 적용해보자

<div>
<img src="/opengl_course/note/images/15_pbr_equirect_to_cube_result.png" width="75%"/>
</div>

---

## Equirectangular-to-Cube

- `CubeTexture` 클래스 리팩토링
  - 이미지 없이 특정 픽셀 타입의 빈 텍스처 생성

```cpp [6-7, 13-16, 21, 24-27]
CLASS_PTR(CubeTexture)
class CubeTexture {
public:
  static CubeTextureUPtr CreateFromImages(
    const std::vector<Image*> images);
  static CubeTextureUPtr Create(int width, int height,
    uint32_t format, uint32_t type = GL_UNSIGNED_BYTE);
  ~CubeTexture();

  const uint32_t Get() const { return m_texture; }
  void Bind() const;

  int GetWidth() const { return m_width; }
  int GetHeight() const { return m_height; }
  uint32_t GetFormat() const { return m_format; }
  uint32_t GetType() const { return m_type; }

private:
  CubeTexture() {}
  bool InitFromImages(const std::vector<Image*> images);
  void Init(int width, int height, int format, uint32_t type);

  uint32_t m_texture { 0 };
  int m_width { 0 };
  int m_height { 0 };
  uint32_t m_format { GL_RGBA };
  uint32_t m_type { GL_UNSIGNED_BYTE };
};
```

---

## Equirectangular-to-Cube

- `CubeTexture` 클래스 리팩토링
  - internal format으로부터 image format 유추하는 기능을 별도의 함수로 변환하자

```cpp
static GLenum GetImageFormat(uint32_t internalFormat) {
  GLenum imageFormat = GL_RGBA;
  if (internalFormat == GL_DEPTH_COMPONENT) {
    imageFormat = GL_DEPTH_COMPONENT;        
  }
  else if (internalFormat == GL_RGB ||
    internalFormat == GL_RGB16F ||
    internalFormat == GL_RGB32F) {
    imageFormat = GL_RGB;
  }
  else if (internalFormat == GL_RG ||
    internalFormat == GL_RG16F ||
    internalFormat == GL_RG32F) {
    imageFormat = GL_RG;
  }
  else if (internalFormat == GL_RED ||
    internalFormat == GL_R ||
    internalFormat == GL_R16F ||
    internalFormat == GL_R32F) {
    imageFormat = GL_RED;
  }
  return imageFormat;
}
```

---

## Equirectangular-to-Cube

- `CubeTexture` 클래스 리팩토링
  - internal format으로부터 image format 유추하는 기능을 별도의 함수로 변환하자

```cpp [8]
void Texture::SetTextureFormat(int width, int height,
  uint32_t format, uint32_t type) {
  m_width = width;
  m_height = height;
  m_format = format;
  m_type = type;

  GLenum imageFormat = GetImageFormat(m_format);
  glTexImage2D(GL_TEXTURE_2D, 0, m_format,
    m_width, m_height, 0,
    imageFormat, m_type,
    nullptr);
}
```

---

## Equirectangular-to-Cube

- `CubeTexture` 클래스 리팩토링

```cpp
CubeTextureUPtr CubeTexture::Create(int width, int height,
  uint32_t format, uint32_t type) {
  auto texture = CubeTextureUPtr(new CubeTexture());
  texture->Init(width, height, format, type);
  return std::move(texture);
}
```

---

## Equirectangular-to-Cube

- `CubeTexture` 클래스 리팩토링

```cpp [11-14, 26-27]
bool CubeTexture::InitFromImages(const std::vector<Image*> images) {
  glGenTextures(1, &m_texture);
  Bind();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  m_width = images[0]->GetWidth();
  m_height = images[0]->GetHeight();
  m_type = images[0]->GetBytePerChannel() == 4 ? GL_FLOAT : GL_UNSIGNED_BYTE;
  m_format = images[0]->GetBytePerChannel() == 4 ? GL_RGB16F : GL_RGB;

  for (uint32_t i = 0; i < (uint32_t)images.size(); i++) {
    auto image = images[i];
    GLenum format = GL_RGBA;
    switch (image->GetChannelCount()) {
      default: break;
      case 1: format = GL_RED; break;
      case 2: format = GL_RG; break;
      case 3: format = GL_RGB; break;
    }

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_format,
      m_width, m_height, 0, format, m_type, image->GetData());
  }

  return true;
}
```

---

## Equirectangular-to-Cube

- `CubeTexture` 클래스 리팩토링

```cpp
void CubeTexture::Init(int width, int height, int format, uint32_t type) {
  glGenTextures(1, &m_texture);
  Bind();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  m_width = width;
  m_height = height;
  m_type = type;
  m_format = format;
  GLenum imageFormat = GetImageFormat(m_format);

  for (uint32_t i = 0; i < 6; i++) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_format,
      m_width, m_height, 0, imageFormat, m_type, nullptr);
  }
}
```

---

## Equirectangular-to-Cube

- `CubeFramebuffer` 클래스 추가
  - 큐브 맵을 color attachment로 사용하는 framebuffer

```cpp
CLASS_PTR(CubeFramebuffer);
class CubeFramebuffer {
public:
  static CubeFramebufferUPtr Create(const CubeTexturePtr colorAttachment);
  ~CubeFramebuffer();

  const uint32_t Get() const { return m_framebuffer; }
  void Bind(int cubeIndex = 0) const;
  const CubeTexturePtr GetColorAttachment() const { return m_colorAttachment; }

private:
  CubeFramebuffer() {}
  bool InitWithColorAttachment(const CubeTexturePtr& colorAttachment);

  uint32_t m_framebuffer { 0 };
  uint32_t m_depthStencilBuffer { 0 };
  CubeTexturePtr m_colorAttachment;
};
```

---

## Equirectangular-to-Cube

- `CubeFramebuffer` 클래스 메소드 구현

```cpp
CubeFramebufferUPtr CubeFramebuffer::Create(const CubeTexturePtr colorAttachment) {
  auto framebuffer = CubeFramebufferUPtr(new CubeFramebuffer());
  if (!framebuffer->InitWithColorAttachment(colorAttachment))
    return nullptr;
  return std::move(framebuffer);
}

CubeFramebuffer::~CubeFramebuffer() {
  if (m_depthStencilBuffer) {
    glDeleteRenderbuffers(1, &m_depthStencilBuffer);
  }
  if (m_framebuffer) {
    glDeleteFramebuffers(1, &m_framebuffer);
  }
}

void CubeFramebuffer::Bind(int cubeIndex) const {
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubeIndex,
    m_colorAttachment->Get(), 0);
}

bool CubeFramebuffer::InitWithColorAttachment(const CubeTexturePtr& colorAttachment) {
  m_colorAttachment = colorAttachment;
  glGenFramebuffers(1, &m_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  glFramebufferTexture2D(GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    m_colorAttachment->Get(), 0);

  int width = m_colorAttachment->GetWidth();
  int height = m_colorAttachment->GetHeight();

  glGenRenderbuffers(1, &m_depthStencilBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
    GL_RENDERBUFFER, m_depthStencilBuffer);

  auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (result != GL_FRAMEBUFFER_COMPLETE) {
    SPDLOG_ERROR("failed to create framebuffer: 0x{:04x}", result);
    return false;
  }

  Framebuffer::BindToDefault();

  return true;
}
```

---

## Equirectangular-to-Cube

- `CubeFramebuffer` 클래스 디자인 이슈
  - 6개의 framebuffer를 만들어서 각각마다 하나의
    color attachment를 갖도록 하기
  - 하나의 framebuffer에 cubemap의 각 면을
    color attachment 0 ~ 6에 설정하기
  - 하나의 framebuffer에 cubemap의 각 면을
    color attachment 0번에만 바꿔가며 설정하기

---

## Equirectangular-to-Cube

- `Context` 클래스에 큐브맵 멤버 추가

```cpp [3]
  TextureUPtr m_hdrMap;
  ProgramUPtr m_sphericalMapProgram;
  CubeTexturePtr m_hdrCubeMap;
```

---

## Equirectangular-to-Cube

- `Context::Init()`에서 큐브맵의 각 면마다 HDR 이미지 렌더링

```cpp
  m_hdrCubeMap = CubeTexture::Create(512, 512, GL_RGB16F, GL_FLOAT);
  auto cubeFramebuffer = CubeFramebuffer::Create(m_hdrCubeMap);
  auto projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
  std::vector<glm::mat4> views = {
    glm::lookAt(glm::vec3(0.0f),
      glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f),
      glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    glm::lookAt(glm::vec3(0.0f),
      glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f),
      glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f),
      glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
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

  Framebuffer::BindToDefault();
  glViewport(0, 0, m_width, m_height);
```

---

## Equirectangular-to-Cube

- `shader/skybox_hdr.vs` 추가
  - `shader/skybox.vs`와 거의 유사

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;
out vec3 localPos;

uniform mat4 projection;
uniform mat4 view;

void main() {
  localPos = aPos;
  mat4 rotView = mat4(mat3(view)); // remove translation
  vec4 clipPos = projection * rotView * vec4(localPos, 1.0);
  gl_Position = clipPos.xyww;
}
```

---

## Equirectangular-to-Cube

- skybox 쉐이더 구현 트릭
  - `mat4 rotView = mat4(mat3(view));`
    - view 행렬이 가지고 있는 translation 부분이 사라지도록 만듬
  - `clipPos.xyww`
    - fragment shader에서 z값이 항상 1이 되도록 만듬

---

## Equirectangular-to-Cube

- `shader/skybox_hdr.fs` 추가

```glsl
#version 330 core

out vec4 fragColor;
in vec3 localPos;

uniform samplerCube cubeMap;

void main() {
  vec3 envColor = texture(cubeMap, localPos).rgb;
  envColor = envColor / (envColor + vec3(1.0));   // reinhard
  envColor = pow(envColor, vec3(1.0/2.2));    // to sRGB
  fragColor = vec4(envColor, 1.0);
}
```

---

## Equirectangular-to-Cube

- `Context` 클래스에 skybox 렌더링용 프로그램 멤버 추가

```cpp [4]
  TextureUPtr m_hdrMap;
  ProgramUPtr m_sphericalMapProgram;
  CubeTexturePtr m_hdrCubeMap;
  ProgramUPtr m_skyboxProgram;
```

---

## Equirectangular-to-Cube

- `Context::Render()`에서 HDR skybox 렌더링

```cpp
  glDepthFunc(GL_LEQUAL);
  m_skyboxProgram->Use();
  m_skyboxProgram->SetUniform("projection", projection);
  m_skyboxProgram->SetUniform("view", view);
  m_skyboxProgram->SetUniform("cubeMap", 0);
  m_hdrCubeMap->Bind();
  m_box->Draw(m_skyboxProgram.get());
  glDepthFunc(GL_LESS);
```

---

## Equirectangular-to-Cube

- 빌드 및 실행

<div>
<img src="/opengl_course/note/images/15_pbr_hdr_map_skybox_result.png" width="70%"/>
</div>

---

## Cubemap Convolution

- Cubemap으로부터 convolution을 통한 diffuse irradiance 계산

<div>
<img src="/opengl_course/note/images/15_pbr_diffuse_irradiance_figure.png" width="40%"/>
</div>

---

## Cubemap Convolution

- 구면 좌표계를 이용한 적분식으로의 변경

<div>
<img src="/opengl_course/note/images/15_pbr_diffuse_irradiance_spherical_figure.png" width="50%"/>
</div>

<div>
<img src="/opengl_course/note/images/15_pbr_diffuse_irradiance_spherical.png" width="80%"/>
</div>

---

## Cubemap Convolution

- `shader/diffuse_irradiance.fs` 추가

```glsl
#version 330 core
out vec4 fragColor;
in vec3 localPos;

uniform samplerCube cubeMap;
const float PI = 3.14159265359;

void main() {
  // the sample direction equals the hemisphere’s orientation
  vec3 normal = normalize(localPos);
  vec3 up = vec3(0.0, 1.0, 0.0);
  vec3 right = normalize(cross(up, normal)); 
  up = cross(normal, right);

  vec3 irradiance = vec3(0.0);
  float sampleDelta = 0.025;
  float nrSamples = 0.0;
  for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
    for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
      // spherical to cartesian (in tangent space)
      vec3 tangentSample = vec3(
        sin(theta) * cos(phi),
        sin(theta) * sin(phi),
        cos(theta));
      // tangent space to world
      vec3 sampleVec = tangentSample.x * right +
        tangentSample.y * up +
        tangentSample.z * normal;
      irradiance += texture(cubeMap, sampleVec).rgb * cos(theta) * sin(theta);
      nrSamples++;
    }
  }
  irradiance = PI * irradiance * (1.0 / float(nrSamples));

  fragColor = vec4(irradiance, 1.0);
}
```

---

## Cubemap Convolution

- `Context`에 멤버 추가

```cpp [5-6]
  TextureUPtr m_hdrMap;
  ProgramUPtr m_sphericalMapProgram;
  CubeTexturePtr m_hdrCubeMap;
  ProgramUPtr m_skyboxProgram;
  CubeTexturePtr m_diffuseIrradianceMap;
  ProgramUPtr m_diffuseIrradianceProgram;
```

---

## Cubemap Convolution

- `Context::Init()`에서 HDR cubemap convolution 렌더링 코드 추가

```cpp []
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

  Framebuffer::BindToDefault();
  glViewport(0, 0, m_width, m_height);
```

---

## Cubemap Convolution

- `Context::Render()`에서 skybox를 HDR cubemap 대신
  diffuse irradiance map으로 대체하여 결과 확인

```cpp [6]
  glDepthFunc(GL_LEQUAL);
  m_skyboxProgram->Use();
  m_skyboxProgram->SetUniform("projection", projection);
  m_skyboxProgram->SetUniform("view", view);
  m_skyboxProgram->SetUniform("cubeMap", 0);
  m_diffuseIrradianceMap->Bind();
  m_box->Draw(m_skyboxProgram.get());
  glDepthFunc(GL_LESS);
```

---

## Cubemap Convolution

- 빌드 및 결과

<div>
<img src="/opengl_course/note/images/15_pbr_diffuse_irradiance_skybox_result.png" width="80%"/>
</div>

---

## Diffuse Irradiance

- Irradiance map 적용
  - 계산된 irradiance map은 indirect light로 보면 된다
    - ambient 색상으로 사용
  - diffuse와 specular가 섞여있으므로 Fresnel 값으로 diffuse 성분만 알아낸다
  - Fresnel 계산시 halfway 대신 normal 사용
  - normal에 roughness 적용 필요

---

## Diffuse Irradiance

- 만들어진 irradiance map을 PBR에 적용해보자
  - `shader/pbr.fs` 수정

```glsl [3-4]
uniform Material material;

uniform samplerCube irradianceMap;
uniform int useIrradiance;

// ...

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
```

---

## Diffuse Irradiance

- `shader/pbr.fs` 수정

```glsl [2-8]
  vec3 ambient = vec3(0.03) * albedo * ao;
  if (useIrradiance == 1) {
    vec3 kS = FresnelSchlickRoughness(dotNV, F0, roughness);
    vec3 kD = 1.0 - kS;
    vec3 irradiance = texture(irradianceMap, fragNormal).rgb;
    vec3 diffuse = irradiance * albedo;
    ambient = (kD * diffuse) * ao;
  }
```

---

## Diffuse Irradiance

- `Context` 클래스 멤버 추가 및 다시 PBR을 위한 파라미터로 변경

```cpp [6, 9-11]
  struct Light {
    glm::vec3 position { glm::vec3(0.0f, 0.0f, 0.0f) };
    glm::vec3 color { glm::vec3(1.0f, 1.0f, 1.0f) };
  };
  std::vector<Light> m_lights;
  bool m_useDiffuseIrradiance { true };

  struct Material {
    glm::vec3 albedo { glm::vec3(1.0f, 1.0f, 1.0f) };
    float roughness { 0.5f };
    float metallic { 0.5f };
    float ao { 0.1f };
  };
  Material m_material;
```

---

## Diffuse Irradiance

- `Context::Init()`의 프로그램 초기화 코드 변경 및 텍스처 로딩 코드 삭제

```cpp [3-4]
  m_simpleProgram = Program::Create(
    "./shader/simple.vs", "./shader/simple.fs");
  m_pbrProgram = Program::Create(
    "./shader/pbr.vs", "./shader/pbr.fs");
```

---

## Diffuse Irradiance

- `Context::Render()`, `DrawScene()` 렌더링 코드 변경

```cpp [6-9, 23, 31-32]
  // Context::Render()
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  m_pbrProgram->Use();
  m_pbrProgram->SetUniform("viewPos", m_cameraPos);
  m_pbrProgram->SetUniform("material.ao", m_material.ao);
  m_pbrProgram->SetUniform("material.albedo", m_material.albedo);
  m_pbrProgram->SetUniform("useIrradiance", m_useDiffuseIrradiance ? 1 : 0);
  m_pbrProgram->SetUniform("irradianceMap", 0);
  m_diffuseIrradianceMap->Bind();
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


  // Context::DrawScene()
  program->SetUniform("transform", transform);
  program->SetUniform("modelTransform", modelTransform);
  program->SetUniform("material.roughness", (float)(i + 1) / (float)sphereCount);
  program->SetUniform("material.metallic", (float)(j + 1) / (float)sphereCount);
```

---

## Diffuse Irradiance

- ImGui 윈도우에 재질 및 irradiance map 사용 여부 조작 UI 추가

```cpp
  if (ImGui::CollapsingHeader("material")) {
    ImGui::ColorEdit3("mat.albedo", glm::value_ptr(m_material.albedo));
    ImGui::SliderFloat("mat.roughness", &m_material.roughness, 0.0f, 1.0f);
    ImGui::SliderFloat("mat.metallic", &m_material.metallic, 0.0f, 1.0f);
    ImGui::SliderFloat("mat.ao", &m_material.ao, 0.0f, 1.0f);
  }
  ImGui::Checkbox("use irradiance", &m_useDiffuseIrradiance);
```

---

## Diffuse Irradiance

- 빌드 및 결과
  - irradiance 사용 여부에 따른 결과 차이를 비교해보자

<div>
<img src="/opengl_course/note/images/15_pbr_diffuse_irradiance_result.png" width="70%"/>
</div>

---

## Specular IBL

- Specular 항에 대한 precomputing 방법
  - Diffuse 항과는 달리 상수항이 아니므로 그대로 미리 계산할 수는 없다
  - Epic Games' split sum approximation
    - Light 부분과 BRDF 부분을 나누어 계산해둔다

<div>
<img src="/opengl_course/note/images/15_pbr_specular_split_eq.png" width="60%"/>
</div>

---

## Specular IBL

- Pre-filtered environment map
  - Mipmap 형태로 roughness에 따라 계산을 달리하여 저장
  - Monte-Carlo integration 방식의 계산

---

## Prefiltered map

- Roughness 값에 따라 달리 계산하는 이유
  - Specular의 경우 roughness에 따라 반사되는 방향이 분산되기 떄문 (Specular lobe)

<div>
<img src="/opengl_course/note/images/15_pbr_prefiltered_light_roughness.png" width="80%"/>
</div>

---

## Prefiltered map

- [Monte-Carlo integration](https://en.wikipedia.org/wiki/Monte_Carlo_method)
  - 랜덤한 샘플 수집을 이용한 통계적인 적분 계산 기법
  - Quasi-Monte Carlo integration
    - 좀 더 빠른 수렴을 위해 low-discrepancy sequence를 사용

<div>
<img src="/opengl_course/note/images/15_pbr_low_discrepancy_sequence.png" width="40%"/>
</div>

---

## Prefiltered map

- `CubeTexture` 클래스에 mipmap 생성 기능 추가

```cpp
// in CubeTexture class declaration
  void GenerateMipmap() const;

// in texture.cpp
void CubeTexture::GenerateMipmap() const {
  Bind();
  glTexParameteri(GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}
```

---

## Prefiltered map

- `CubeFramebuffer` 클래스에 mipmap 레벨 설정 기능 추가

```cpp
// in CubeTexture class declaration
  static CubeFramebufferUPtr Create(
    const CubeTexturePtr colorAttachment, uint32_t mipLevel = 0);
  // ...
  bool InitWithColorAttachment(
    const CubeTexturePtr& colorAttachment, uint32_t mipLevel);
  // ...
  uint32_t m_mipLevel { 0 };

// in CubeFramebuffer::Bind()
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubeIndex,
    m_colorAttachment->Get(), m_mipLevel);

// in CubeFramebuffer::InitWithColorAttachment()
  m_colorAttachment = colorAttachment;
  m_mipLevel = mipLevel;
  glGenFramebuffers(1, &m_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  glFramebufferTexture2D(GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    m_colorAttachment->Get(), m_mipLevel);

  int width = m_colorAttachment->GetWidth() >> m_mipLevel;
  int height = m_colorAttachment->GetHeight() >> m_mipLevel;
```

---

## Prefiltered map

- `shader/prefiltered_light.fs` 추가

```cpp
#version 330 core

out vec4 fragColor;
in vec3 localPos;

uniform samplerCube cubeMap;
uniform float roughness;

const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits) {
  bits = (bits << 16u) | (bits >> 16u); 
  bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u); 
  bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u); 
  bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u); 
  bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
  return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N) {
  return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
  float a = roughness*roughness;
  float phi = 2.0 * PI * Xi.x;
  float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
  float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

  // from spherical coordinates to cartesian coordinates
  vec3 H;
  H.x = cos(phi) * sinTheta;
  H.y = sin(phi) * sinTheta;
  H.z = cosTheta;

  // from tangent-space vector to world-space sample vector
  vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
  vec3 tangent = normalize(cross(up, N));
  vec3 bitangent = cross(N, tangent);

  vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
  return normalize(sampleVec);
}

void main() {
  vec3 N = normalize(localPos);
  vec3 R = N;
  vec3 V = R;

  const uint SAMPLE_COUNT = 1024u;
  float totalWeight = 0.0;
  vec3 prefilteredColor = vec3(0.0);
  for(uint i = 0u; i < SAMPLE_COUNT; ++i) {
    vec2 Xi = Hammersley(i, SAMPLE_COUNT);
    vec3 H = ImportanceSampleGGX(Xi, N, roughness);
    vec3 L = normalize(2.0 * dot(V, H) * H - V);
    float NdotL = max(dot(N, L), 0.0);
    if(NdotL > 0.0) {
      prefilteredColor += texture(cubeMap, L).rgb * NdotL;
      totalWeight += NdotL;
    }
  }
  prefilteredColor = prefilteredColor / totalWeight;

  fragColor = vec4(prefilteredColor, 1.0);
}
```

---

## Prefiltered map

- `Context` 클래스에 prefiltered map을 위한 프로그램 및 텍스처 멤버 추가

``` cpp [7-8]
  TextureUPtr m_hdrMap;
  ProgramUPtr m_sphericalMapProgram;
  CubeTexturePtr m_hdrCubeMap;
  ProgramUPtr m_skyboxProgram;
  CubeTexturePtr m_diffuseIrradianceMap;
  ProgramUPtr m_diffuseIrradianceProgram;
  CubeTexturePtr m_preFilteredMap;
  ProgramUPtr m_preFilteredProgram;
```

---

## Prefiltered map

- `Context::Init()`에서 prefiltered map 계산

```cpp
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
```

---

## Prefiltered Map

- `shader/skybox_hdr.fs`를 임시 수정
  - 모든 mip level의 결과를 관찰하기 위함

```glsl [2, 5-6]
uniform samplerCube cubeMap;
uniform float roughness;

void main() {
  // vec3 envColor = texture(cubeMap, localPos).rgb;
  vec3 envColor = textureLod(cubeMap, localPos, roughness * 4).rgb;
```

---

## Prefiltered Map

- `Context::Render()`의 skybox 그리는 코드 임시 수정

```cpp [3, 7-8]
  glDepthFunc(GL_LEQUAL);
  m_skyboxProgram->Use();
  m_skyboxProgram->SetUniform("roughness", m_material.roughness);
  m_skyboxProgram->SetUniform("projection", projection);
  m_skyboxProgram->SetUniform("view", view);
  m_skyboxProgram->SetUniform("cubeMap", 0);
  // m_hdrCubeMap->Bind()
  m_preFilteredMap->Bind();
  m_box->Draw(m_skyboxProgram.get());
  glDepthFunc(GL_LESS);
```

---

## Prefiltered Map

- 빌드 및 결과
  - material.roughness 값을 바꾸면서 결과 관찰
  - cube의 연결 부분에 경계선이 눈에 띔
  - 밝은 점 패턴이 눈에 띄는 현상

<div>
<img src="/opengl_course/note/images/15_pbr_prefiltered_light_seam.png" width="60%"/>
</div>

---

## Prefiltered Map

- 경계선 없애기
  - `Context::Init()`에서 `GL_TEXTURE_CUBE_MAP_SEAMLESS` 활성화

```cpp [4]
bool Context::Init() {
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
```

---

## Prefiltered Map

- 점 패턴 없애기
  - roughness에 따라 mipmap으로부터 샘플링

```glsl [1-10, 14-23]
float DistributionGGX(vec3 normal, vec3 halfDir, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float dotNH = max(dot(normal, halfDir), 0.0);
  float dotNH2 = dotNH * dotNH;

  float num = a2;
  float denom = (dotNH2 * (a2 - 1.0) + 1.0);
  return a2 / (PI * denom * denom);
}

// ... on computing convolution
if(NdotL > 0.0) {
  float D = DistributionGGX(N, H, roughness);
  float NdotH = max(dot(N, H), 0.0);
  float HdotV = max(dot(H, V), 0.0);
  float pdf = (D * NdotH / (4.0 * HdotV)) + 0.0001;
  float resolution = 512.0; // resolution of source cubemap (per face)
  float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
  float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
  float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

  prefilteredColor += textureLod(cubeMap, L, mipLevel).rgb * NdotL;
  totalWeight += NdotL;
}
```

---

## Prefiltered Map

- 점 패턴 없애기
  - `Context::Init()`에서 HDR cube map 생성 후 mipmap 생성

```cpp [9]
  m_hdrMap->Bind();
  glViewport(0, 0, 512, 512);
  for (int i = 0; i < (int)views.size(); i++) {
    cubeFramebuffer->Bind(i);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_sphericalMapProgram->SetUniform("transform", projection * views[i]);
    m_box->Draw(m_sphericalMapProgram.get());
  }
  m_hdrCubeMap->GenerateMipmap();
```

---

## Prefiltered Map

- 빌드 및 결과
  - 경계선이 사라지고 점 패턴이 사라진 부드러운 prefiltered map

<div>
<img src="/opengl_course/note/images/15_pbr_prefiltered_light_seamless.png" width="60%"/>
</div>

---

## BRDF Lookup Table

- `shader/brdf_lookup.vs` 추가

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
out vec2 texCoord;

void main() {
  gl_Position = transform * vec4(aPos, 1.0);
  texCoord = aTexCoord;
}
```

---

## BRDF Lookup Table

- `shader/brdf_lookup_fs` 추가

```glsl
#version 330 core
out vec2 fragColor;
in vec2 texCoord;

const float PI = 3.14159265359;

float RadicalInverse_VdC(uint bits) {
  bits = (bits << 16u) | (bits >> 16u); 
  bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u); 
  bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u); 
  bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u); 
  bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
  return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N) {
  return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness) {
  float a = roughness*roughness;
  float phi = 2.0 * PI * Xi.x;
  float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
  float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

  // from spherical coordinates to cartesian coordinates
  vec3 H;
  H.x = cos(phi) * sinTheta;
  H.y = sin(phi) * sinTheta;
  H.z = cosTheta;

  // from tangent-space vector to world-space sample vector
  vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
  vec3 tangent = normalize(cross(up, N));
  vec3 bitangent = cross(N, tangent);

  vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
  return normalize(sampleVec);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
  float a = roughness;
  float k = (a * a) / 2.0;
  float nom = NdotV;
  float denom = NdotV * (1.0 - k) + k;
  return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);
  return ggx1 * ggx2;
}

vec2 IntegrateBRDF(float NdotV, float roughness) {
  vec3 V;
  V.x = sqrt(1.0 - NdotV*NdotV);
  V.y = 0.0;
  V.z = NdotV;

  float A = 0.0;
  float B = 0.0;

  vec3 N = vec3(0.0, 0.0, 1.0);

  const uint SAMPLE_COUNT = 1024u;
  for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
    vec2 Xi = Hammersley(i, SAMPLE_COUNT);
    vec3 H = ImportanceSampleGGX(Xi, N, roughness);
    vec3 L = normalize(2.0 * dot(V, H) * H - V);

    float NdotL = max(L.z, 0.0);
    float NdotH = max(H.z, 0.0);
    float VdotH = max(dot(V, H), 0.0);

    if(NdotL > 0.0) {
      float G = GeometrySmith(N, V, L, roughness);
      float G_Vis = (G * VdotH) / (NdotH * NdotV);
      float Fc = pow(1.0 - VdotH, 5.0);
      A += (1.0 - Fc) * G_Vis;
      B += Fc * G_Vis;
    }
  }
  A /= float(SAMPLE_COUNT);
  B /= float(SAMPLE_COUNT);
  return vec2(A, B);
}

void main() {
  vec2 integratedBRDF = IntegrateBRDF(texCoord.x, texCoord.y);
  fragColor = integratedBRDF;
}
```

---

## BRDF Lookup Table

- `Context`에 lookup table 저장용 텍스처 및 프로그램 멤버 추가

```cpp
  TexturePtr m_brdfLookupMap;
  ProgramUPtr m_brdfLookupProgram;
```

---

## BRDF Lookup Table

- `Context::Init()`에서 lookup table 생성

```cpp
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
```

---

## BRDF Lookup Table

- ImGui 윈도우 내에서 lookup table 생성 결과 확인

```cpp
  float w = ImGui::GetContentRegionAvailWidth();
  ImGui::Image((ImTextureID)m_brdfLookupMap->Get(), ImVec2(w, w));
```

---

## BRDF Lookup Table

- 빌드 및 결과

<div>
<img src="/opengl_course/note/images/15_pbr_brdf_lookup_table.png" width="80%"/>
</div>

---

## IBL Reflectance

- `shader/pbr.fs` 수정

```glsl
uniform samplerCube irradianceMap;
uniform samplerCube preFilteredMap;
uniform sampler2D brdfLookupTable;
uniform int useIBL;

// ... after direct lighting computation
  vec3 ambient = vec3(0.03) * albedo * ao;
  if (useIBL == 1) {
    vec3 kS = FresnelSchlickRoughness(dotNV, F0, roughness);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, fragNormal).rgb;
    vec3 diffuse = irradiance * albedo;

    vec3 R = reflect(-viewDir, fragNormal);
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 preFilteredColor = textureLod(preFilteredMap, R,
        roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBrdf = texture(brdfLookupTable, vec2(dotNV, roughness)).rg;
    vec3 specular = preFilteredColor * (kS * envBrdf.x + envBrdf.y);

    ambient = (kD * diffuse + specular) * ao;
  }
  vec3 color = ambient + outRadiance;
```

---

## IBL Reflectance

- `shader/skybox_hdr.fs`를 다시 원복
- `Context::m_useDiffuseIrradiance`의 이름을 `m_useIBL`로 변경
- `Context::Render()`에서 준비된 pre-filtered environment map과
  BRDF lookup table을 PBR 렌더링때 uniform으로 전달

---

## IBL Reflectance

```cpp [6-16]
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
```

---

## IBL Reflectance

- 빌드 및 결과

<div>
<img src="/opengl_course/note/images/15_pbr_final_result.png" width="80%"/>
</div>

---

## IBL Reflectance

- [Cerberus by A. Maximov](http://artisaverb.info/PBT.html)

<div>
<img src="/opengl_course/note/images/15_pbr_final_result_gun.png" width="100%"/>
</div>

---

## Additional Notes

- Pre-computed IBL
  - 실행때마다 계산할 필요가 없음
    - 보통은 미리 계산해서 디스크에 저장해둠
    - HDR 포맷으로 저장
    - [cmftStudio](https://github.com/dariomanesku/cmftStudio)나
      [IBLBaker](https://github.com/derkreature/IBLBaker) 같은 소프트웨어를
      써도 무방
- [Light probes](https://chetanjags.wordpress.com/2015/08/26/image-based-lighting/)

---

## Reference

- [**lifeisforu**의 그냥 그런 블로그](https://lifeisforu.tistory.com/category/Physically%20Based%20Rendering)
  - 매우 좋은 PBR 관련 설명글 (한국어)

---

## Congratulation!
### 수고하셨습니다!