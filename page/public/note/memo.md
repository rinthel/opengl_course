# Reference: OpenGL Super Bible, 7th edition

- 중요 내용 요약

### Ch1. Introduction

##### opengl and graphics pipeline

- 고효율의 제품 생산: scalability & parallelism
- 프로그램이 OpenGL을 통해 커맨드를 그래픽스 하드웨어로 전달
- 그래픽스 하드웨어는 최대한 고속으로 이를 처리해줌
- OpenGL의 목표
  - 프로그램과 하드웨어 간의 abstraction layer
  - 어떤 GPU인지, 어떤식으로 동작하는지 상관없이 동작을 보장
- 설계시의 주안점
  - 너무 high / low abstraction level이 되지 않게 한다
    - OS/system specific 한 문제는 숨겨야 함 (high)
    - hardware 접근성은 보장해야함 (low)
    - high하면 프로그램 작성은 쉬워지지만 하드웨어가 가진 고급 기능은 쓰기 힘듬
    - 너무 low하면 프로그램 작성시 신경쓸게 너무 많아짐
- 현재의 GPU
  - teraflop 단위의 컴퓨팅 파워
  - 초당 수백 GB를 읽을 수 있는 메모리
  - high rate, high resolution, multiple display
  - 매우 유연한 GPU, 다양한 일에 사용 가능
- GPU 구조
  - shader core: shader 실행 가능한 작은 프로그래밍 가능한 프로세서, 수백개 달려 있음
  - 각각은 throughput이 작지만,

### 첫번째 숙제

- Git에 익숙해지기
  - Github 계정 만들기
  - 컴퓨터에 fork client 설치
  - Github에 opengl_exercise_01 repository 생성
    - private repository로 할것
  - window 띄우는 데모 commit
  - 프로젝트 생성 과정부터 단계별로 commit을 진행할 것
  - README.md를 작성하여 빌드 방법에 대해 설명할 것
  - `rinthel` 계정을 collaborator로 등록할 것
