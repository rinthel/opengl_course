# Computer Graphics and OpenGL

---

## 컴퓨터 그래픽스?

- Digitally synthesizing and manipulating visual content
  - from [wikipedia](https://en.wikipedia.org/wiki/Computer_graphics_(computer_science))
- 보통은 3D 컴퓨터 그래픽스를 일컫지만, 실은 2D / 이미지 처리를 포괄하는 용어

---

## 연구분야

- Image / Video Processing
- Modeling
- Rendering
- Character Animation
- Physics-based simulation

---

## 응용분야

- 영화 / 애니메이션 / 게임
- GUI / 시각화

---

## OpenGL

- 3D 그래픽을 화면에 표현하기 위해 필요한 기본적인 기능들을 모은 오픈 API
- SGI (실리콘 그래픽스) 사의 그래픽 라이브러리 GL의 오픈소스화
- 1992년 첫 버전 발표
- 버전업을 거쳐 현재 4.6 까지 업데이트됨

---

## OpenGL을 가지고 할 수 있는 일들

- Real-time 2D/3D rendering
  - 2D/3D game engine
  - VR/AR engine
  - Photo-realistic rendering
  - GUI component rendering
  - General-purpose computing
    - image processing, neural network

---

## OpenGL을 가지고 할 수 없는 것들

- Window management (OS part)
- Input (keyboard/mouse) management (OS part)
- GUI framework (only for drawing)

---

## OpenGL 외 그래픽스 / GPU API

- Graphics API
  - DirectX: Microsoft - Windows
  - Metal: Apple - macOS, iOS
  - Vulkan: Next generation of OpenGL

- GPGPU API
  - CUDA: nVidia
  - OpenCL: Khronos

---

## 왜 OpenGL 인가?

- Good starting point
- Cross-platform
- Steady used API

---

## 왜 Vulkan이 아닌가?

- Lower-level than OpenGL
- Hard to learn without Graphics / GPU knowledge
- 하지만 관심이 있다면 공부해보세요. 다양한 장점이 있습니다.

---

## OpenGL의 특징

- Low-level API
- Cross-language
- Cross-platform
- Standard / Specification only
  - Khronos Group에서 API 스펙을 관리
  - 실제 OpenGL driver는 각 벤더에서 스펙에 맞게 개발

---

## OpenGL Version

- 1.x: Immediate mode, Standard pipeline
- 2.x: Vertex/fragment shader
- 3.x: New context, **Forward compatibility**
- 4.x: Compute shader

---

## 이 수업시간에 다룰 것

- OpenGL 3.3
  - 가장 널리 사용되어온 OpenGL 버전
  - Core-profile
    - 2.x 이전 기능 중 불필요하고 비효율적인 기능 제거
    - 새로운 형태의 OpenGL context
  - Forward compatibility
    - 이후 버전에서도 동작할 것을 보장

---

## Congratulation!
### 수고하셨습니다!