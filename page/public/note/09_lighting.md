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

## Congratulation!
### 수고하셨습니다!
