type state = {
  email: string,
  password: string,
  hasValidationError: bool,
  errorList: list(string),
};

type action =
  | Login(bool, list(string))
  | UpdateEmail(string)
  | UpdatePassword(string)
  | LoginPending;

let goToRegister = (router, event) => {
  event->ReactEvent.Mouse.preventDefault;
  DirectorRe.setRoute(router, "/register");
};

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

let loginUser = (route, event, {ReasonReact.state, send}) => {
  event->ReactEvent.Mouse.preventDefault;
  let reduceByAuthResult = (_status, jsonPayload) =>
    jsonPayload
    |> Js.Promise.then_(json => {
         let newUser = JsonRequests.checkForErrors(json);
         let updatedState =
           switch (newUser) {
           | Some(errors) => {
               ...state,
               hasValidationError: true,
               errorList: errors |> JsonRequests.convertErrorsToList,
             }
           | None =>
             let loggedIn = JsonRequests.parseNewUser(json);
             LocalStorage.saveToken(loggedIn.user.token);
             LocalStorage.saveUser(loggedIn.user.username, loggedIn.user.bio);
             DirectorRe.setRoute(route, "/home");
             {...state, hasValidationError: false};
           };
         /* TODO: Create a reducer to do nothing with succesful login so the state doesn't try to update */
         send(_ =>
           Login(updatedState.hasValidationError, updatedState.errorList)
         )
         |> Js.Promise.resolve;
       });
  JsonRequests.authenticateUser(reduceByAuthResult, toJson(state)) |> ignore;
  send(_ => LoginPending);
};

let component = ReasonReact.reducerComponent("Login");

let make = (~router, _children) => {
  ...component,
  initialState: () => {
    email: "",
    password: "",
    hasValidationError: false,
    errorList: [],
  },
  reducer: (action, state) =>
    switch (action) {
    | UpdateEmail(email) => ReasonReact.Update({...state, email})
    | UpdatePassword(password) => ReasonReact.Update({...state, password})
    | Login(hasError, errorList) =>
      ReasonReact.Update({...state, hasValidationError: hasError, errorList})
    | LoginPending => ReasonReact.NoUpdate
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
              <a href="#" onClick={goToRegister(router)}>
                {ReasonReact.string("Need an account?")}
              </a>
            </p>
            {
              if (state.hasValidationError) {
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
                onClick={event => self.handle(loginUser(router))}
                className="btn btn-lg btn-primary pull-xs-right">
                {ReasonReact.string("Sign in")}
              </button>
            </form>
          </div>
        </div>
      </div>
    </div>,
};
