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

- PBR surface model
- Textured PBR

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

---

## Congratulation!
### 수고하셨습니다!