type state = unit;

type action =
  | GoToCreateArticle
  | GoToHome
  | GoToLogin
  | GoToProfile
  | GoToRegistration
  | GoToSettings;

let pointer = ReactDOMRe.Style.make(~cursor="pointer", ());

let displayUsername = () => {
  let (_, _, optionalName) = LocalStorage.getUser();
  Belt.Option.getWithDefault(optionalName, "Username Missing");
};

let component = ReasonReact.reducerComponent("Header");

let make = (~router, ~token, _children) => {
  ...component,
  initialState: () => (),
  reducer: (action, _state: unit) =>
    switch (action) {
    | GoToCreateArticle =>
      ReasonReact.SideEffects(
        (_ => DirectorRe.setRoute(router, "article/create")),
      )
    | GoToHome =>
      ReasonReact.SideEffects((_ => DirectorRe.setRoute(router, "/home")))
    | GoToLogin =>
      ReasonReact.SideEffects((_ => DirectorRe.setRoute(router, "/login")))
    | GoToProfile =>
      ReasonReact.SideEffects((_ => DirectorRe.setRoute(router, "/profile")))
    | GoToRegistration =>
      ReasonReact.SideEffects(
        (_ => DirectorRe.setRoute(router, "/register")),
      )
    | GoToSettings =>
      ReasonReact.SideEffects(
        (_ => DirectorRe.setRoute(router, "/settings")),
      )
    },
  render: ({send}) =>
    <div>
      <nav className="navbar navbar-light">
        <div className="container">
          <a className="navbar-brand" href="index.html">
            {ReasonReact.string("conduit")}
          </a>
          {
            switch (token) {
            | Some(_) =>
              <ul className="nav navbar-nav pull-xs-right">
                <li className="nav-item">
                  <a
                    className="nav-link active"
                    style=pointer
                    href="#"
                    onClick=(
                      event => {
                        event->ReactEvent.Mouse.preventDefault;
                        send(GoToHome);
                      }
                    )>
                    {ReasonReact.string("Home")}
                  </a>
                </li>
                <li className="nav-item">
                  <a
                    className="nav-link"
                    style=pointer
                    href="#"
                    onClick=(
                      event => {
                        event->ReactEvent.Mouse.preventDefault;
                        send(GoToCreateArticle);
                      }
                    )>
                    <i className="ion-compose" />
                    {ReasonReact.string(" New Post")}
                  </a>
                </li>
                <li className="nav-item">
                  <a
                    className="nav-link"
                    style=pointer
                    href="#"
                    onClick=(
                      event => {
                        event->ReactEvent.Mouse.preventDefault;
                        send(GoToSettings);
                      }
                    )>
                    <i className="ion-gear-a" />
                    {ReasonReact.string(" Settings")}
                  </a>
                </li>
                <li className="nav-item">
                  <a
                    className="nav-link"
                    style=pointer
                    href="#"
                    onClick=(
                      event => {
                        event->ReactEvent.Mouse.preventDefault;
                        send(GoToProfile);
                      }
                    )>
                    {ReasonReact.string(displayUsername())}
                  </a>
                </li>
              </ul>
            | None =>
              <ul className="nav navbar-nav pull-xs-right">
                <li className="nav-item">
                  <a
                    className="nav-link active"
                    style=pointer
                    href="#"
                    onClick=(
                      event => {
                        event->ReactEvent.Mouse.preventDefault;
                        send(GoToHome);
                      }
                    )>
                    {ReasonReact.string("Home")}
                  </a>
                </li>
                <li className="nav-item">
                  <a
                    className="nav-link"
                    style=pointer
                    href="#"
                    onClick=(
                      event => {
                        event->ReactEvent.Mouse.preventDefault;
                        send(GoToLogin);
                      }
                    )>
                    {ReasonReact.string("Log in")}
                  </a>
                </li>
                <li className="nav-item">
                  <a
                    className="nav-link"
                    style=pointer
                    href="#"
                    onClick=(
                      event => {
                        event->ReactEvent.Mouse.preventDefault;
                        send(GoToRegistration);
                      }
                    )>
                    {ReasonReact.string("Sign up")}
                  </a>
                </li>
              </ul>
            }
          }
        </div>
      </nav>
    </div>,
};
