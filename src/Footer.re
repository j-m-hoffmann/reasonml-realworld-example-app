let component = ReasonReact.statelessComponent("Footer");

let conduit_message = () =>
  <div>
    {ReasonReact.string("An interactive learning project from ")}
    <a href="https://thinkster.io"> {ReasonReact.string("Thinkster")} </a>
    {ReasonReact.string(". Code &amp; design licensed under MIT.")}
  </div>;

let make = _children => {
  ...component,
  render: _self =>
    <footer>
      <div className="container">
        <a href="/" className="logo-font"> {ReasonReact.string("conduit")} </a>
        <span className="attribution"> {conduit_message()} </span>
      </div>
    </footer>,
};
