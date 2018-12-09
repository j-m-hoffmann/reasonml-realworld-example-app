let component = ReasonReact.statelessComponent("Footer");

let make = _children => {
  ...component,
  render: _self =>
    <footer>
      <div className="container">
        <a href="/" className="logo-font">
          {ReasonReact.string("conduit")}
        </a>
        <span className="attribution">
          {ReasonReact.string("An interactive learning project from ")}
          <a href="https://thinkster.io">
            {ReasonReact.string("Thinkster")}
          </a>
          {ReasonReact.string(". Code & design licensed under MIT.")}
        </span>
      </div>
    </footer>,
};
