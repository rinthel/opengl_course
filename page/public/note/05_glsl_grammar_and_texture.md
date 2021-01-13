# GLSL Grammar and Texture

---

## GLSL

- OpenGL Shader Language
  - Shader: GPU 상에서 동작하는 그림을 그리기 위한 작은 프로그램
  - GLSL: OpenGL에서 shader를 작성하기 위해 제공하는 C 기반 언어
  - 그 외의 대표적인 shader language
    - HLSL: DirectX용 shader language
    - Metal: Metal용 shader language
    - cg: nVidia가 제시한 shader language

---

## GLSL

- GLSL source code의 대략적인 구조

```glsl
#version version_number

in type in_variable_name;
in type in_variable_name;

out type out_variable_name;

uniform type uniform_name;

void main() {
  // process input(s) and do some weird graphics stuff ...
  // output processed stuff to output variable
  out_variable_name = weird_stuff_we_processed;
}
```

---

## GLSL

- GLSL에서 사용 가능한 data type
  - 기본 타입: `int, float, double, uint, bool`
  - **벡터 타입**:
    - `vecX`: `float`형 벡터
    - `bvecX`: `bool`형 벡터
    - `ivecX`: `int`형 벡터
    - `uvecX`: `uint`형 벡터
    - `dvecX`: `double`형 벡터
  - `X`에는 2, 3, 4 사용 가능

---

## GLSL

- GLSL에서 사용 가능한 data type
  - **행렬 타입**:
    - `matX`: `float`형 행렬
    - `bmatX`: `bool`형 행렬
    - `imatX`: `int`형 행렬
    - `umatX`: `uint`형 행렬
    - `dmatX`: `double`형 행렬
  - `X`에는 2, 3, 4 사용 가능

---

## Vector

- 벡터 초기값 선언
  - 생성자 사용
  - 다른 벡터를 섞어서 사용 가능

```glsl
vec2 vect = vec2(0.5, 0.7);
vec4 result = vec4(vect, 0.0, 0.0);
vec4 otherResult = vec4(result.xyz, 1.0);
```

---

## Vector

- 벡터 원소 접근
  - `.x`, `.y`, `.z`, `.w` 인덱스를 사용
  - swizzling 가능
    - 얻어오고 싶은 인덱스를 연속으로 쓰기
    - `.xyz` => `vec3`

```glsl
vec2 someVec;
vec4 differentVec = someVec.xyxx;
vec3 anotherVec = differentVec.zyw;
vec4 otherVec = someVec.xxxx + anotherVec.yxzy;
```

---

## 첫 예제 준비

- `triangle_example`의 코드를 복사하여 `glsl_example` 프로젝트 생성
- `CMakeLists.txt`의 프로젝트명 변경
- VSCode로 열어서 빌드 및 실행 확인

---

## Congratulation!
### 수고하셨습니다!
