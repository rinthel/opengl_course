import { Link } from "react-router-dom";

function Home() {
  return (
    <div>
      <h1> OpenGL course by Rinthel </h1>
      <p>
        Welcome OpenGL course page!
      </p>
      <h3>Links</h3>
      <ul>
        <li><a href="https://github.com/rinthel/opengl_course">Github repo</a></li>
      </ul>
      <h3>Slide notes</h3>
      <ul>
        <li><Link to="1">1. Introduction</Link></li>
      </ul>
    </div>
  );
}

export default Home;