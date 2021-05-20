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
  m_program->SetUniform("transform", transform);
  m_program->SetUniform("modelTransform", modelTransform);
  m_planeMaterial->SetToProgram(program);
  m_box->Draw(program);

  modelTransform =
    glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
    glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
  transform = projection * view * modelTransform;
  m_program->SetUniform("transform", transform);
  m_program->SetUniform("modelTransform", modelTransform);
  m_box1Material->SetToProgram(program);
  m_box->Draw(program);

  modelTransform =
    glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
    glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
  transform = projection * view * modelTransform;
  m_program->SetUniform("transform", transform);
  m_program->SetUniform("modelTransform", modelTransform);
  m_box2Material->SetToProgram(program);
  m_box->Draw(program);

  modelTransform =
    glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 1.75f, -2.0f)) *
    glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
  transform = projection * view * modelTransform;
  m_program->SetUniform("transform", transform);
  m_program->SetUniform("modelTransform", modelTransform);
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