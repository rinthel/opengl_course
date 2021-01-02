import { useEffect, useRef } from 'react';
import { Link, useRouteMatch } from "react-router-dom";

// @ts-ignore
import Reveal from 'reveal.js';
// @ts-ignore
import Markdown from 'reveal.js/plugin/markdown/markdown.esm.js';
// @ts-ignore
import Highlight from 'reveal.js/plugin/highlight/highlight.esm.js';
import 'reveal.js/dist/reveal.css';
import 'reveal.js/dist/theme/moon.css';
import 'reveal.js/plugin/highlight/monokai.css';

function Slide() {
  type Params = {
    id: string
  };
  const match = useRouteMatch();
  const param = match.params as Params;
  const id = param.id;
  const dataPath = `/note/${id}.md`;

  console.log("match:", match);
  const deckRef = useRef(null);
  useEffect(() => {
      if (deckRef.current)
        return;
      let deck = new Reveal({
        plugins: [Markdown, Highlight]
      });
      deck.initialize({
        embedded: true,
        center: true,
        control: true,
        controlsTutorial: true,
        slideNumber: true,
        showSlideNumber: "all",
        hash: true,
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
