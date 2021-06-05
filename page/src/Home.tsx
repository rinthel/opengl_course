import { Link } from "react-router-dom";
import styled from "styled-components";

const Background = styled.div`
  padding: 20px;
`;

const Page = styled.div`
  margin: auto;
  width: 800px;
  padding: 40px;
  background-color: #f8f8f8;
  border-radius:20px;
`;

function Home() {
  return (
    <Background>
    <Page>
      <h1> OpenGL course by Rinthel </h1>
      <p>
        Rinthel의 OpenGL 코스 페이지에 오신 분들을 환영합니다.
        이 페이지는 강원대학교 2021년 1학기 "컴퓨터 그래픽스" 수업과
        관련된 슬라이드 노트를 담고 있습니다.
      </p>
      <h3>관련 링크</h3>
      <ul>
        <li><a href="https://github.com/rinthel/opengl_course">Github 저장소</a></li>
      </ul>
      <h3>참고 문헌</h3>
      <ul>
        <li><a href="https://learnopengl.com">learnopengl.com</a></li>
      </ul>
      <h3>슬라이드 노트</h3>
      <ul>
        <li><Link to="01_introduction">1. Introduction</Link></li>
        <li><Link to="02_computer_graphics_and_opengl">2. Computer Graphics and OpenGL</Link></li>
        <li><Link to="03_development_environment">3. Development Environment</Link></li>
        <li>4. Graphics Pipeline and Shader</li>
        <ul>
          <li><Link to="04_graphics_pipeline_and_shader_01">Part 1</Link></li>
          <li><Link to="04_graphics_pipeline_and_shader_02">Part 2</Link></li>
        </ul>
        <li><Link to="05_glsl">5. GLSL</Link></li>
        <li><Link to="06_texture">6. Texture</Link></li>
        <li><Link to="07_transformation">7. Transformation</Link></li>
        <li><Link to="08_imgui">8. ImGui</Link></li>
        <li><Link to="09_lighting">9. Lighting</Link></li>
        <li><Link to="10_object">10. Object Loading</Link></li>
        <li><Link to="11_advanced_feature_01">11. Advanced Feature Part. 1</Link></li>
        <li><Link to="12_advanced_feature_02">12. Advanced Feature Part. 2</Link></li>
        <li><Link to="13_advanced_lighting_01">13. Advanced Lighting Part. 1</Link></li>
        <li><Link to="14_advanced_lighting_02">14. Advanced Lighting Part. 2</Link></li>
        <li><Link to="15_physics_based_rendering">15. Physics-Based Rendering</Link></li>
      </ul>
      <h3>문의</h3>
      <h4>권지용 (rinthel AT gmail.com)</h4>
    </Page>
    </Background>
  );
}

export default Home;