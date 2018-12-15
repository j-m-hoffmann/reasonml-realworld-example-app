type state = {
  email: string,
  password: string,
  loginFailed: bool,
  errorList: list(string),
};

type action =
  | GoToRegister
  | LogIn
  | LoginSuccessful(AuthResponse.User.t)
  | LoginFailed(list(string))
  | UpdateEmail(string)
  | UpdatePassword(string);
/*| LoginPending;*/

let toJson = credentials =>
  Json.Encode.(
    object_([
      (
        "user",
        object_([
          ("email", string(credentials.email)),
          ("password", string(credentials.password)),
        ]),
      ),
    ])
  );

let component = ReasonReact.reducerComponent("Login");

let make = (~router, _children) => {
  ...component,
  initialState: () => {
    email: "",
    password: "",
    loginFailed: false,
    errorList: [],
  },
  reducer: (action, state) =>
    switch (action) {
    | GoToRegister =>
      ReasonReact.SideEffects(
        (_ => DirectorRe.setRoute(router, "/register")),
      )
    | LogIn =>
      ReasonReact.SideEffects(
        self =>
          Request.User.logIn(toJson(state), ~f=json =>
            AuthResponse.(
              switch (checkForErrors(json)) {
              | None => self.send(LoginSuccessful(User.fromJson(json)))
              | Some(errors) =>
                self.send(LoginFailed(Errors.toList(errors)))
              }
            )
          )
          |> ignore,
        /*send(LoginPending);*/
      )
    | LoginSuccessful(user) =>
      ReasonReact.SideEffects(
        (
          _ => {
            LocalStorage.saveToken(user.token);
            LocalStorage.saveUser(user.bio, user.image, user.username);
            DirectorRe.setRoute(router, "/home");
          }
        ),
      )
    | LoginFailed(errorList) =>
      ReasonReact.Update({...state, errorList, loginFailed: true})
    | UpdateEmail(email) => ReasonReact.Update({...state, email})
    | UpdatePassword(password) => ReasonReact.Update({...state, password})
    /*| LoginPending => ReasonReact.NoUpdate*/
    },
  render: ({state, send}) =>
    <div className="auth-page">
      <div className="container page">
        <div className="row">
          <div className="col-md-6 offset-md-3 col-xs-12">
            <h1 className="text-xs-center">
              {ReasonReact.string("Sign in")}
            </h1>
            <p className="text-xs-center">
              <a
                href="#"
                onClick={
                  event => {
                    event->ReactEvent.Mouse.preventDefault;
                    send(GoToRegister);
                  }
                }>
                {ReasonReact.string("Need an account?")}
              </a>
            </p>
            {
              if (state.loginFailed) {
                <ErrorMessages errors={state.errorList} />;
              } else {
                ReasonReact.null;
              }
            }
            <form>
              <fieldset className="form-group">
                <input
                  type_="text"
                  className="form-control form-control-lg"
                  placeholder="Email"
                  value={state.email}
                  onChange={
                    event =>
                      send(
                        UpdateEmail(ReactEvent.Form.target(event)##value),
                      )
                  }
                />
              </fieldset>
              <fieldset className="form-group">
                <input
                  type_="password"
                  className="form-control form-control-lg"
                  placeholder="Password"
                  value={state.password}
                  onChange={
                    event =>
                      send(
                        UpdatePassword(ReactEvent.Form.target(event)##value),
                      )
                  }
                />
              </fieldset>
              <button
                onClick={
                  event => {
                    event->ReactEvent.Mouse.preventDefault;
                    send(LogIn);
                  }
                }
                className="btn btn-lg btn-primary pull-xs-right">
                {ReasonReact.string("Sign in")}
              </button>
            </form>
          </div>
        </div>
      </div>
    </div>,
};
