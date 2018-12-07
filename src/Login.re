type state = {
  email: string,
  password: string,
  loginFailed: bool,
  errorList: list(string),
};

type action =
  | GoToRegister
  | LoginSuccessful(User.t)
  | LoginFailed(list(string))
  | UpdateEmail(string)
  | UpdatePassword(string);
/*| LoginPending;*/

/*module Encode = {*/
/*let encodeUserCredentials = creds => {*/
/*open! Json.Encode;*/
/*object_([*/
/*("email", string(creds.email)),*/
/*("password", string(creds.password)),*/
/*]);*/
/*};*/
/*let user = topLevelUser =>*/
/*Json.Encode.(object_([("user", encodeUserCredentials(topLevelUser))]));*/

/*let currentUser = (username, bio) =>*/
/*Json.Encode.[("username", string(username)), ("bio", string(bio))];*/
/*};*/

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

let loginUser = (event, {ReasonReact.state, send}) => {
  event->ReactEvent.Mouse.preventDefault;
  open JsonRequests;
  let reduceByAuthResult = (_status, jsonPayload) =>
    jsonPayload
    |> Js.Promise.then_(json =>
         checkForErrors(json)
         ->(
             fun
             | None => send(LoginSuccessful(parseNewUser(json).user))
             | Some(errors) =>
               send(LoginFailed(convertErrorsToList(errors)))
           )
         ->Js.Promise.resolve
       );
  authenticateUser(reduceByAuthResult, toJson(state))->ignore;
  /*send(LoginPending);*/
};

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
    | LoginSuccessful(user) =>
      ReasonReact.SideEffects(
        (
          _ => {
            LocalStorage.saveToken(user.token);
            LocalStorage.saveUser(user.username, user.bio, user.image);
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
  render: ({state, send, handle}) =>
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
                onClick={handle(loginUser)}
                className="btn btn-lg btn-primary pull-xs-right">
                {ReasonReact.string("Sign in")}
              </button>
            </form>
          </div>
        </div>
      </div>
    </div>,
};
