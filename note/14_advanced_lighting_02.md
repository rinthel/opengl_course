# Advanced Lighing Part 2

---

## HDR

- 그래픽 하드웨어가 다룰 수 있는 색상값의 범위
  - 0.0 ~ 1.0 사이의 값
  - fragment shader에서 1.0보다 큰 값이 들어오면 clamp 처리됨

<div>
<img src="/opengl_course/note/images/14_hdr_clamped.png" width="35%"/>
</div>

---

## HDR

- naive solution
  - light strength를 줄여서 1.0을 넘지 못하게 한다
  - 비현실적인 light parameter 설정을 하게 되어 별로 좋지 못한 결과가 발생
- better solution
  - 일단 일시적으로 1.0을 넘는 값들도 허용한다
  - 마지막에 색상값들을 0.0 ~ 1.0 사이에 맞게 조절한다

---

## HDR

- HDR
  - High Dynamic Range
  - 모니터에서 표현 가능한 색상 범위를 벗어나는 값을 저장하고 사용하는 기법

---

## HDR

- HDR Image / Tone mapping
  - 이미지 처리 분야에서 노출값을 달리한 여러 장의 이미지를 합치는 방법
  - 인간의 눈이 밝기에 적응하는 방식과 유사함

<div>
<img src="/opengl_course/note/images/14_hdr_image.png" width="80%"/>
</div>

---

## HDR

- Floating point framebuffer
  - framebuffer 생성 시 사용할 텍스처가 1.0 이상의 값을
    저장할 수 있도록 한다
  - `GL_RGBA16F`, `GL_RGBA32F`

```cpp
glBindTexture(GL_TEXTURE_2D, colorBuffer);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
    SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
```

---

## HDR

- Tone mapping
  - Floating point framebuffer에 그려진 texture를 0 ~ 1 범위에
    맞춰서 다시 화면에 그리기
  - Reinhard tone mapping: 모든 범위를 0 ~ 1 사이에 표현

```glsl
void main() {
  const float gamma = 2.2;
  vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
  // reinhard tone mapping
  vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
  // gamma correction
  mapped = pow(mapped, vec3(1.0 / gamma));
}
```

---

## HDR

- Exposure tone mapping
  - 노출값을 파라미터로 사용하여 그에 따라 밝기를 조절

```glsl
uniform float exposure;
void main() {
  const float gamma = 2.2;
  vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
  // exposure tone mapping
  vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
  // gamma correction mapped = pow(mapped, vec3(1.0 / gamma));
  FragColor = vec4(mapped, 1.0);
}
```

---

## HDR

- Exposure tone mapping
  - 노출값에 따른 결과 화면 예시

<div>
<img src="/opengl_course/note/images/14_hdr_exposure.png" width="90%"/>
</div>

---

## Bloom

- Bloom
  - glow effect를 표현하게 해주는 후처리 효과 중 하나
  - 1 이상의 밝은 빛을 시각적으로 표현해주는 좋은 방법

<div>
<img src="/opengl_course/note/images/14_bloom_comparison.png" width="80%"/>
</div>

---

## Bloom

- Algorithm overview

<div>
<img src="/opengl_course/note/images/14_bloom_overview.png" width="80%"/>
</div>

---

## Bloom

- 빛나는 영역을 추출하는 방법
  - Multiple render target을 이용
    - Framebuffer에 두 개 이상의 color attachment를 설정

```glsl
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
```

---

## Bloom

- Framebuffer에 여러 Texture를 color attachment로 설정하기

```glsl
// set up floating point framebuffer to render scene to
unsigned int hdrFBO;
glGenFramebuffers(1, &hdrFBO);
glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

unsigned int colorBuffers[2];
glGenTextures(2, colorBuffers);
for (unsigned int i = 0; i < 2; i++) {
  glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
    SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // attach texture to framebuffer
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
    GL_TEXTURE_2D, colorBuffers[i], 0);
}

unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
glDrawBuffers(2, attachments);
```

---

## Bloom

- 기본 장면과 빛나는 영역을 함께 렌더링

```glsl
#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
[...]
void main() {
  [...]
  // first do normal lighting calculations and output results
  FragColor = vec4(lighting, 1.0);
  // if fragment output is higher than threshold, output brightness color
  float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
  if(brightness > 1.0)
    BrightColor = vec4(FragColor.rgb, 1.0);
  else
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
```

---

## Bloom

- Gaussian blur
  - 대표적인 blurring 알고리즘 중 하나
  - 모든 픽셀들에 대해서 다음을 수행
    - 이웃하는 픽셀들 색상의 가중치 평균
    - 가중치는 Gaussian 함수를 바탕으로 계산

<div>
<img src="/opengl_course/note/images/14_bloom_gaussian.png" width="30%"/>
</div>

---

## Bloom

- Gaussian blur
  - 3x3 크기의 커널을 사용한 Gaussian blur 연산 과정

<div>
<img src="/opengl_course/note/images/14_bloom_gaussian_filter.jpg" width="40%"/>
</div>

---

## Bloom

- Gaussian blur
  - 연산량
    - 모든 픽셀에 대한 연산
    - 커널의 크기 k x k
      - k^2 번의 곱셈과 덧셈
    - 커널의 크기가 커질수록 연산량이 증가

---

## Bloom

- Separate Gaussian blur
  - Tow-pass Gaussian blur
  - 커널의 형태가 대칭형일 경우 x축 / y축으로 나눠서 처리할 수 있음
  - k^2 번의 연산이 2k 번으로 감소

<div>
<img src="/opengl_course/note/images/14_bloom_separate_gaussian.png" width="60%"/>
</div>

---

## Bloom

- 9x9 크기의 Separate Gaussian filter shader

```glsl
#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D image;
uniform bool horizontal;

uniform float weight[5] = float[] (
  0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216
);
void main() {
  vec2 tex_offset = 1.0 / textureSize(image, 0); // size of single texel
  vec3 result = texture(image, TexCoords).rgb * weight[0]; // this fragment
  if(horizontal) {
    for(int i = 1; i < 5; ++i) {
      result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
      result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }
  } else {
    for(int i = 1; i < 5; ++i) {
      result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
      result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
    }
  }
  FragColor = vec4(result, 1.0);
}
```

---

## Bloom

- Separate Gaussian filter를 여러번에 걸쳐 적용
  - 두 개의 Framebuffer에 번갈아가며 렌더링

```glsl
int amount = 10;
shaderBlur.use();
for (unsigned int i = 0; i < amount; i++) {
  glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i % 2]);
  shaderBlur.setInt("horizontal", horizontal);
  glBindTexture(GL_TEXTURE_2D,
    i == 0
    ? colorBuffers[1]
    : pingpongBuffers[(i - 1) % 2]);
  RenderQuad();
}
glBindFramebuffer(GL_FRAMEBUFFER, 0);
```

---

## Bloom

- 기본 장면과 blur된 장면 합치고 tone mapping 적용

```glsl
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;
void main() {
  const float gamma = 2.2;
  vec3 hdrColor = texture(scene, TexCoords).rgb;
  vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
  hdrColor += bloomColor;                              // additive blending
  vec3 result = vec3(1.0) - exp(-hdrColor * exposure); // tone mapping
  result = pow(result, vec3(1.0 / gamma));             // gamma correction
  FragColor = vec4(result, 1.0);
}
```

---

## Bloom

- Bloom 적용 결과 비교

<div>
<img src="/opengl_course/note/images/14_bloom_test_scene.png" width="40%"/>
<img src="/opengl_course/note/images/14_bloom_test_result.png" width="40%"/>
</div>

---

## Deferred Shading

- Forward shading
  - 현재까지 우리가 렌더링한 방법
  - light source가 여러 개고 object도 여러 개일 경우 성능이 빠르게 저하될 수 있음
    - 화면 밖으로 벗어난 object
    - 화면 안에 들어있으나 앞의 object에 가려진 경우

---

## Deferred Shading

- Deferred shading
  - Defer: 미루다
  - light 계산과 관련된 모든 연산을 뒤로 미루자

---

## Deferred Shading

- Geometry pass
  - light 계산에 필요한 geometric / texture data를 먼저 렌더링
  - G-buffer

<div>
<img src="/opengl_course/note/images/14_deferred_g_buffer.png" width="40%"/>
</div>

---

## Deferred Shading

- Lighting pass
  - G-buffer에 저장된 정보를 바탕으로 lighting 계산

<div>
<img src="/opengl_course/note/images/14_deferred_process.png" width="75%"/>
</div>

---

## Deferred Shading

- G-buffer
  - lighting 계산과 관련된 모든 정보를 담은 텍스처 뭉치
    - 3D world space position
    - Albedo (= diffuse color, 기본 배색)
    - 3D normal
    - Specular intensity
    - Light source position & color
    - View position vector

---

## Deferred Shading

- `Framebuffer` 클래스 리팩토링
  - 여러 개의 color attachment를 사용할 수 있도록 하기 위한 코드 수정

```cpp [3-4, 10-15, 19-20, 24]
class Framebuffer {
public:
  static FramebufferUPtr Create(
    const std::vector<TexturePtr>& colorAttachments);
  static void BindToDefault();
  ~Framebuffer();

  const uint32_t Get() const { return m_framebuffer; }
  void Bind() const;
  int GetColorAttachmentCount() const {
    return (int)m_colorAttachments.size();
  }
  const TexturePtr GetColorAttachment(int index = 0) const {
    return m_colorAttachments[index];
  }

private:
  Framebuffer() {}
  bool InitWithColorAttachments(
    const std::vector<TexturePtr>& colorAttachments);

  uint32_t m_framebuffer { 0 };
  uint32_t m_depthStencilBuffer { 0 };
  std::vector<TexturePtr> m_colorAttachments;
};
```

---

## Deferred Shading

- `Framebuffer` 클래스 리팩토링

```cpp [2, 4]
FramebufferUPtr Framebuffer::Create(
  const std::vector<TexturePtr>& colorAttachments) {
  auto framebuffer = FramebufferUPtr(new Framebuffer());
  if (!framebuffer->InitWithColorAttachments(colorAttachments))
    return nullptr;
  return std::move(framebuffer);
}
```

---

## Deferred Shading

- `Framebuffer` 클래스 리팩토링

```cpp [2, 7-11, 13-22, 26-27]
bool Framebuffer::InitWithColorAttachments(
  const std::vector<TexturePtr>& colorAttachments) {
  m_colorAttachments = colorAttachments;
  glGenFramebuffers(1, &m_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  for (size_t i = 0; i < m_colorAttachments.size(); i++) {
    glFramebufferTexture2D(GL_FRAMEBUFFER,
      GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
      m_colorAttachments[i]->Get(), 0);
  }

  if (m_colorAttachments.size() > 0) {
    std::vector<GLenum> attachments;
    attachments.resize(m_colorAttachments.size());
    for (size_t i = 0; i < m_colorAttachments.size(); i++)
      attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    glDrawBuffers(m_colorAttachments.size(), attachments.data());
  }

  int width = m_colorAttachments[0]->GetWidth();
  int height = m_colorAttachments[0]->GetHeight();

  glGenRenderbuffers(1, &m_depthStencilBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
    width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
```

---

## Deferred Shading

- `Context::Init()`에서 호출하고 있는 framebuffer 생성 코드 수정

```cpp
  m_framebuffer = Framebuffer::Create({
      Texture::Create(width, height, GL_RGBA),
  });
```

---

## Deferred Shading

- `Texture::SetTextureFormat()` 코드 수정
  - `GL_RGBA16F` 텍스처 생성시 image format은 `GL_RGBA`를 사용

```cpp [8-16, 20]
void Texture::SetTextureFormat(int width, int height,
  uint32_t format, uint32_t type) {
  m_width = width;
  m_height = height;
  m_format = format;
  m_type = type;

  GLenum imageFormat = GL_RGBA;
  if (m_format == GL_DEPTH_COMPONENT) {
    imageFormat = GL_DEPTH_COMPONENT;        
  }
  else if (m_format == GL_RGB ||
    m_format == GL_RGB16F ||
    m_format == GL_RGB32F) {
    imageFormat = GL_RGB;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, m_format,
    m_width, m_height, 0,
    imageFormat, m_type,
    nullptr);
}
```

---

## Deferred Shading

- `shader/defer_geo.vs` 추가

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 modelTransform;

out vec3 normal;
out vec2 texCoord;
out vec3 position;

void main() {
  gl_Position = transform * vec4(aPos, 1.0);
  normal = (transpose(inverse(modelTransform)) * vec4(aNormal, 0.0)).xyz;
  texCoord = aTexCoord;
  position = (modelTransform * vec4(aPos, 1.0)).xyz;
}
```

---

## Deferred Shading

- `shader/defer_geo.fs` 추가

```glsl
#version 330 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 position;
in vec3 normal;
in vec2 texCoord;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
};
uniform Material material;

void main() {
  // store the fragment position vector in the first gbuffer texture
  gPosition = vec4(position, 1.0);
  // also store the per-fragment normals into the gbuffer
  gNormal = vec4(normalize(normal), 1.0);
  // and the diffuse per-fragment color
  gAlbedoSpec.rgb = texture(material.diffuse, texCoord).rgb;
  // store specular intensity in gAlbedoSpec’s alpha component
  gAlbedoSpec.a = texture(material.specular, texCoord).r;
}
```

---

## Deferred Shading

- `Context`에 deferred shading 관련 멤버 변수 추가

```cpp
  // deferred shading
  FramebufferUPtr m_deferGeoFramebuffer;
  ProgramUPtr m_deferGeoProgram;
```

---

## Deferred Shading

- `Context::Reshape()`에서 G-buffer framebuffer 생성

```cpp
  m_deferGeoFramebuffer = Framebuffer::Create({
    Texture::Create(width, height, GL_RGBA16F, GL_FLOAT),
    Texture::Create(width, height, GL_RGBA16F, GL_FLOAT),
    Texture::Create(width, height, GL_RGBA, GL_UNSIGNED_BYTE),
  });
```

---

## Deferred Shading

- `Context::Init()`에서 program 초기화

```cpp
  m_deferGeoProgram = Program::Create(
    "./shader/defer_geo.vs", "./shader/defer_geo.fs");
```

---

## Deferred Shading

- `Context::Render()`에서 deferred shading 첫 번째 패스 렌더링

```cpp
  m_deferGeoFramebuffer->Bind();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_width, m_height);
  m_deferGeoProgram->Use();
  DrawScene(view, projection, m_deferGeoProgram.get());

  Framebuffer::BindToDefault();
  glViewport(0, 0, m_width, m_height);
```

---

## Deferred Shading

- `Context::Render()`에서 결과 확인을 위한 ImGui 윈도우 추가

```cpp
  if (ImGui::Begin("G-Buffers")) {
    const char* bufferNames[] = {
      "position", "normal", "albedo/specular",
    };
    static int bufferSelect = 0;
    ImGui::Combo("buffer", &bufferSelect, bufferNames, 3);
    float width = ImGui::GetContentRegionAvailWidth();
    float height = width * ((float)m_height / (float)m_width);
    auto selectedAttachment =
      m_deferGeoFramebuffer->GetColorAttachment(bufferSelect);
    ImGui::Image((ImTextureID)selectedAttachment->Get(),
      ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
  }
  ImGui::End();
```

---

## Deferred Shading

- 빌드 및 결과
  - ImGui에서 buffer를 바꿔가면서 결과 확인

<div>
<img src="/opengl_course/note/images/14_deferred_geo_pass_result.png" width="70%"/>
</div>

---

- idea
- g-buffer
- deferred lighting pass
- combining deferred with forward
- many lights
- deferred v.s. forward

---

## SSAO

- idea
- sample buffer
- normal-oriented hemisphere
- random kernel rotation
- SSAO shader
- ambient occulsion blur

---

## Congratulation!
### 수고하셨습니다!