# Transformation

---

## 개요

- 화면에서 오브젝트를 임의의 위치에 그려낼 방법
  - vertex buffer object의 vertex position 값을 매번 수정한다
    - 가능하지만, 비효율적임
  - 선형 변환 (linear transformation) 을 이용한다 
    - 가장 기초적이고 일반적인 방법
    - 기본적인 선형 대수에 대한 이해가 필요

---

## Vector

- 크기와 방향을 갖는 값
- 좌표계 상의 벡터: 여러 차원의 좌표값으로 표현
- 덧셈/뺄셈: 각 차원의 원소 별로 덧셈/뺄셈 계산
- 스칼라곱: 각 차원에 스칼라 값을 곱함

![vector add](/opengl_course/note/images/07_vector_add.png)

---

## Vector Length

- 벡터의 크기 (길이) 계산
  - 각 좌표값의 제곱의 합의 제곱근
  - (4, 2) 벡터의 길이
- 단위 벡터 (unit vector)
  - 길이가 1인 벡터

![vector length](/opengl_course/note/images/07_vector_length.png)

---

## Vector Multiplication

- 백터의 내적 (Inner product)
  - Dot product 라고도 함
  - 두 벡터의 길이의 곱 * `cos(두 벡터의 사잇각)`
  - 각 차원별 좌표값의 곱의 합
  - **곱의 결과는 스칼라 값**
  - **기하학적 의미: 투영 (projection)**

![vector dot product](/opengl_course/note/images/07_vector_dot_product.png)

---

## Vector Multiplication

- 백터의 외적 (Cross product)
  - 3차원 벡터에서만 정의
  - **곱의 결과도 3차원 벡터**
  - 기하학적 의미
    - 길이: 두 벡터의 길이의 곱 * `sin(두 벡터의 사잇각)`
    - 즉, 두 벡터가 만들어내는 평행사변형의 넓이
    - 방향: **두 벡터와 수직한 벡터**
  - **교환 법칙이 성립하지 않음**
    - anti-symmetric: A x B = -B x A

---

## Vector Multiplication

![vector cross product figure](/opengl_course/note/images/07_vector_cross_product_figure.png)

![vector cross product](/opengl_course/note/images/07_vector_cross_product.png)

---

## Matrix

- 행렬
- 행(row)과 열(column)로 이뤄진 숫자 묶음으로 구성된 값
- 덧셈/뺄셈: 같은 행과 열에 위치한 원소별로 연산
- 스칼라곱: 스칼라값을 모든 원소에 곱셈 연산

![matrix add](/opengl_course/note/images/07_matrix_add.png)
![matrix scalar](/opengl_course/note/images/07_matrix_scalar.png)

---

## Matrix Multiplication

- A x B
  - A의 column 개수와 B의 row 개수가 일치해야 계산 가능
  - (i, j)의 값: A의 i번째 행 벡터 (row vector) 와
    B의 j번째 열 벡터 (column vector) 간의 내적
  - **교환 법칙 성립 안됨**

![matrix multiplication](/opengl_course/note/images/07_matrix_multiplication.png)

---

## Linear Transformation

- 선형 변환
  - 선형: f(ax + by) = a f(x) + b f(y)
  - 행렬식에서 벡터의 선형 변환
    - f(x) = A x
    - 즉, 선형 변환을 나타내는 행렬과 벡터 간의 곱
  - **여러 개의 선형 변환 적용은 결국 하나의 선형 변환으로 바꿀 수 있다**
    - f(x) = A x
    - g(x) = B x
    - f(g(x)) = A (B x) = C x

---

## Homogeneous Coordinates

- 동차 좌표계
- 표현하고자 하는 차원수보다 1차원 늘어난 벡터로 좌표를 표시하는 방법
  - 마지막 원소 값을 1로 두면 **점**을 표현
  - 3D 좌표 (x, y, z)는 4차원 벡터 (x, y, z, 1)
  - 동차 좌표계에서 (wx, wy, wz, w)는 (x, y, z, 1)과 같은 위치
- 이렇게 하는 이유
  - 평행 이동을 선형 변환으로 표현할 수 있게 됨
  - 원근 투영을 선형 변환으로 표현할 수 있게 됨

---

## Identity Matrix

- 단위 행렬
- 행렬 곱셈의 항등원
  - A x I = I x A = A
- 대각선의 원소값은 1, 나머지는 0
- 출력 벡터는 입력 벡터와 동일해진다

![identity matrix](/opengl_course/note/images/07_identity_matrix.png)

---

## Scaling

- 원점을 기준으로 벡터의 크기를 확대하거나 축소
- 대각 성분에 각 차원별 배율을 지정한 행렬로 표현

![scaling](/opengl_course/note/images/07_scaling.png)

---

## Translation

- 평행이동 행렬
- 4번째 열 벡터에 평행이동 벡터를 가진 행렬로 표현

![translation](/opengl_course/note/images/07_translation.png)

---

## Rotation

- x축에 대한 회전

![rotation x](/opengl_course/note/images/07_rotation_x.png)

---

## Rotation

- y축에 대한 회전

![rotation y](/opengl_course/note/images/07_rotation_y.png)

---

## Rotation

- z축에 대한 회전

![rotation z](/opengl_course/note/images/07_rotation_z.png)

---

## Rotation

- 임의의 축 (Rx, Ry, Rz)에 대한 회전

![rotation axis](/opengl_course/note/images/07_rotation_axis.png)

---

## Combine Matrices

- 주어진 벡터를 2배로 확대한 다음 (1, 2, 3)만큼 평행이동 시키는 행렬

![combine_matrices](/opengl_course/note/images/07_combine_matrices.png)

- 선형 변환은 하나의 행렬로 나타내지므로, 여러 선형 변환을 연속으로
  적용시키는 것은 행렬 곱으로 만들어진 하나의 선형 변환으로 나타낼 수 있다

---

## C++ 행렬 / 벡터 연산

- GLSL의 경우 내부적으로 행렬 및 벡터과 관련된 다양한 기능 및 내부 함수 제공
- C++에는 기본적인 수학적 연산 외에 선형대수 관련 기능은 제공하지 않음
  - 라이브러리를 이용하자

---

## C++ 행렬 / 벡터 연산

- C++ 선형대수 라이브러리
  - Eigen3: 가장 많이 사용되는 C++ 선형대수 라이브러리
    - 일반적인 N차원 선형대수 연산
  - **GLM**: OpenGL Math 라이브러리
    - 3D 그래픽스에 필요한 4차원 선형대수 연산

---

## 예제 준비

- `texture_example` 프로젝트를 복사하여 `transformation_example` 준비
- 프로젝트명 변경 및 빌드 확인

---

## Congratulation!
### 수고하셨습니다!
