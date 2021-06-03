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
  - Metalness: 금속과 비금속의 정도를 0~1사이로 표현한 값
    - 평균적인 비금속 물질의 F0값 0.04와 linear interpolation한 값을
      공식에 적용

```glsl
vec3 fresnelSchlick(float cosTheta, vec3 surfaceColor, float metalness) {
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, surfaceColor.rgb, metalness);
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
  - Metalness: 금속성 / 비금속성
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
  - roughness / metalness 관측용

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
  vec3 fragNormal = normalize(normal);
  vec3 viewDir = normalize(viewPos - fragPos);

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, material.albedo, material.metallic);

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
    float ndf = DistributionGGX(fragNormal, halfDir, material.roughness);
    float geometry = GeometrySmith(fragNormal, viewDir, lightDir, material.roughness);
    vec3 fresnel = FresnelSchlick(max(dot(halfDir, viewDir), 0.0), F0);

    vec3 kS = fresnel;
    vec3 kD = 1.0 - kS;
    kD *= (1.0 - material.metallic);

    float dotNV = max(dot(fragNormal, viewDir), 0.0);
    float dotNL = max(dot(fragNormal, lightDir), 0.0);
    vec3 numerator = ndf * geometry * fresnel;
    float denominator = 4.0 * dotNV * dotNL;
    vec3 specular = numerator / max(denominator, 0.001);

    // add to outgoing radiance Lo
    outRadiance += (kD * material.albedo / PI + specular) * radiance * dotNL;
  }

  vec3 ambient = vec3(0.03) * material.albedo * material.ao;
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

## Diffuse Irradiance

- PBR and HDR
- Cubemap convolution
- PBR and indirect irradiance lighting

---

## Specular IBL

- pre-filtering HDR environment map
- pre-filter convolution artifacts
- pre-computing BRDF
- completing IBL reflectance

---

## Reference

- [**lifeisforu**의 그냥 그런 블로그](https://lifeisforu.tistory.com/category/Physically%20Based%20Rendering)
  - 매우 좋은 PBR 관련 설명글 (한국어)
- [Sampling the GGX Distribution of Visible Normals](http://jcgt.org/published/0007/04/01/paper.pdf)
  - GGX의 뜻이 적혀있는 논문
---

## Congratulation!
### 수고하셨습니다!