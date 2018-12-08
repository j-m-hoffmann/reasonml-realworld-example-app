let component = ReasonReact.statelessComponent("Header");

let pointer = ReactDOMRe.Style.make(~cursor="pointer", ());

let navigateTo = (router, event, routeName) => {
  event->ReactEvent.Mouse.preventDefault;
  DirectorRe.setRoute(router, routeName);
};

let goToRegistration = (router, routeName, event) =>
  navigateTo(router, event, routeName);
let goToHome = (router, routeName, event) =>
  navigateTo(router, event, routeName);
let goToSettings = (router, routeName, event) =>
  navigateTo(router, event, routeName);
let goToCreateArticle = (router, routeName, event) =>
  navigateTo(router, event, routeName);

let displayUsername = () => {
  let (_, _, optionalName) = LocalStorage.getUser();
  switch (optionalName) {
  | Some(name) => name
  | None => "Username Missing"
  };
};

/* This really should be in a reducer component since we are doing a side effect here. */
let displayByLogin = router =>
  switch (LocalStorage.getToken()) {
  | Some(_token) =>
    <a
      className="nav-link"
      style=pointer
      href="#"
      onClick={goToRegistration(router, "/profile")}>
      {ReasonReact.string(displayUsername())}
    </a>
  | None =>
    <a
      className="nav-link"
      style=pointer
      href="#"
      onClick={goToRegistration(router, "/register")}>
      {ReasonReact.string("Sign up")}
    </a>
  };

let make = (~router, _children) => {
  ...component, /* spread the template's other defaults into here  */
  render: _self =>
    <div>
      <nav className="navbar navbar-light">
        <div className="container">
          <a className="navbar-brand" href="index.html">
            {ReasonReact.string("conduit")}
          </a>
          <ul className="nav navbar-nav pull-xs-right">
            <li className="nav-item">
              <a
                className="nav-link active"
                style=pointer
                href="#"
                onClick={goToHome(router, "/home")}>
                {ReasonReact.string("Home")}
              </a>
            </li>
            <li className="nav-item">
              <a
                className="nav-link"
                style=pointer
                href="#"
                onClick={goToCreateArticle(router, "/article/create")}>
                <i className="ion-compose" />
                {ReasonReact.string(" New Post")}
              </a>
            </li>
            <li className="nav-item">
              <a
                className="nav-link"
                style=pointer
                href="#"
                onClick={goToSettings(router, "/settings")}>
                <i className="ion-gear-a" />
                {ReasonReact.string(" Settings")}
              </a>
            </li>
            <li className="nav-item"> {displayByLogin(router)} </li>
          </ul>
        </div>
      </nav>
    </div>,
};
