type state = {
  email: string,
  errorList: list(string),
  validationFailed: bool,
  password: string,
  username: string,
};

type action =
  | GoToLogin
  | SignUp
  | SignUpFailed(list(string))
  | SignUpSuccessful
  | UpdateEmail(string)
  | UpdateName(string)
  | UpdatePassword(string);

let toJson = user =>
  Json.Encode.(
    object_([
      (
        "user",
        object_([
          ("email", string(user.email)),
          ("password", string(user.password)),
          ("username", string(user.username)),
        ]),
      ),
    ])
  );

let component = ReasonReact.reducerComponent("Register");

let make = (~router, _children) => {
  ...component,
  initialState: () => {
    email: "",
    errorList: [],
    validationFailed: false,
    password: "",
    username: "",
  },
  reducer: (action, state) =>
    switch (action) {
    | GoToLogin =>
      ReasonReact.SideEffects((_ => DirectorRe.setRoute(router, "/login")))
    | SignUp =>
      ReasonReact.SideEffects(
        (
          self =>
            Request.registerNewUser(toJson(state), ~f=(_status, payload) =>
              payload
              |> Js.Promise.then_(json => {
                   let newUser = Response.parseNewUser(json);
                   (
                     newUser.errors->Belt.Option.isNone ?
                       self.send(SignUpSuccessful) :
                       self.send(SignUpFailed(newUser->User.errorList))
                   )
                   |> Js.Promise.resolve;
                 })
            )
            |> ignore
        ),
      )
    | SignUpFailed(errorList) =>
      ReasonReact.Update({...state, validationFailed: true, errorList})
    | SignUpSuccessful =>
      ReasonReact.SideEffects((_ => DirectorRe.setRoute(router, "/home")))
    | UpdateEmail(email) => ReasonReact.Update({...state, email})
    | UpdateName(username) => ReasonReact.Update({...state, username})
    | UpdatePassword(value) =>
      ReasonReact.Update({...state, password: value})
    },
  render: ({state, send}) =>
    <div className="auth-page">
      <div className="container page">
        <div className="row">
          <div className="col-md-6 offset-md-3 col-xs-12">
            <h1 className="text-xs-center">
              {ReasonReact.string("Sign up")}
            </h1>
            <p className="text-xs-center">
              <a
                href="#"
                onClick={
                  event => {
                    event->ReactEvent.Mouse.preventDefault;
                    send(GoToLogin);
                  }
                }>
                {ReasonReact.string("Have an account?")}
              </a>
            </p>
            {
              if (state.validationFailed) {
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
                  placeholder="Your Name"
                  value={state.username}
                  onChange={
                    event =>
                      send(UpdateName(ReactEvent.Form.target(event)##value))
                  }
                />
              </fieldset>
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
                    send(SignUp);
                  }
                }
                className="btn btn-lg btn-primary pull-xs-right">
                {ReasonReact.string("Sign up")}
              </button>
            </form>
          </div>
        </div>
      </div>
    </div>,
};
