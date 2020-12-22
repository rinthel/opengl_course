import React from "react";
import { Switch, Route } from "react-router-dom";
import Home from "./Home";
import Slide from "./Slide";
import "./App.css";

// @ts-ignore

function App() {
  return (
    <Switch>
      <Route path="/" exact component={Home}></Route>
      <Route path="/:id">
        <Slide/>
      </Route>
    </Switch>
  );
}

export default App;
