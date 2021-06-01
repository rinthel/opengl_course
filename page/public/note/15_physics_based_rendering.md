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
    빛의 기여도에 따른 에너지의 합

<div>
<img src="/opengl_course/note/images/15_pbr_reflectance_equation.png" width="70%"/>
</div>

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

## Theory

- microfacet model
- energy conservation
- reflectance equation
- BRDF
- Authring PBR materials

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

## Congratulation!
### 수고하셨습니다!