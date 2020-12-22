import { useEffect, useRef } from 'react';
import { Link } from "react-router-dom";

// @ts-ignore
import Reveal from 'reveal.js';
// @ts-ignore
import Markdown from 'reveal.js/plugin/markdown/markdown.esm.js';
import 'reveal.js/dist/reveal.css';
import 'reveal.js/dist/theme/black.css';

function Slide({dataPath = "/note/01_introduction.md"}) {
  const deckRef = useRef(null);
  useEffect(() => {
      if (deckRef.current)
        return;
      let deck = new Reveal({
        plugins: [Markdown]
      });
      deck.initialize({
        embedded: true,
        center: true,
        control: true,
        controlsTutorial: true,
        slideNumber: true,
        showSlideNumber: "all",
      });
      deckRef.current = deck;
  }, [deckRef]);
  
  return (
    <div style={{
      width: "100%",
    }}>
      <div className="reveal" style={{
        position: "absolute",
        top: 0,
        bottom: 0,
      }}>
        <Link to="/">
          <div style={{
            position: "absolute",
            top: 10,
            left: 10,
            width: 100,
            height: 100,
            zIndex: 10,
            fontSize: '1rem',
          }}>
            Home
          </div>
        </Link>
        <div className="slides">
          <section data-markdown={process.env.PUBLIC_URL + dataPath}>
          </section>
        </div>
      </div>
    </div>
  );
}

export default Slide;
