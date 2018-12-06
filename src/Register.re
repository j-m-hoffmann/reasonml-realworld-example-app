type state = {
  email: string,
  errorList: list(string),
  validationFailed: bool,
  password: string,
  username: string,
};

type action =
  | GoToLogin
  | Login
  | Register(bool, list(string))
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

let register = (route, {ReasonReact.state, send}, event) => {
  open JsonRequests;
  event->ReactEvent.Mouse.preventDefault;
  let updateState = (_status, jsonPayload) =>
    jsonPayload
    |> Js.Promise.then_(json => {
         let newUser = parseNewUser(json);
         let newState =
           switch (newUser.errors) {
           | None =>
             DirectorRe.setRoute(route, "/home");
             {...state, validationFailed: false};
           | Some(_errors) => {
               ...state,
               validationFailed: true,
               errorList: newUser->Convert.toErrorListFromResponse,
             }
           };
         send(Register(newState.validationFailed, newState.errorList))
         ->Js.Promise.resolve;
       });
  registerNewUser(updateState, toJson(state))->ignore;
  send(Register(false, ["Hitting server."]));
};

let errorDisplayList = state =>
  List.filter(
    errorMessage => String.length(errorMessage) > 0,
    state.errorList,
  )
  |> List.mapi((acc, errorMessage) =>
       <ul className="error-messages" key={string_of_int(acc)}>
         <li> {ReasonReact.string(errorMessage)} </li>
       </ul>
     );

/* TODO: use the route to go the next home screen when registered successfully */
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
      ReasonReact.SideEffect((_ => DirectorRe.setRoute(router, "/login")))
    | Login => ReasonReact.NoUpdate
    | Register(validationFailed, errorList) =>
      ReasonReact.Update({...state, validationFailed, errorList})
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
                Array.of_list(errorDisplayList(state)) |> ReasonReact.array;
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
                onClick={_ => handle(register(router, self))}
                className="btn btn-lg btn-primary pull-xs-right">
                {ReasonReact.string("Sign up")}
              </button>
            </form>
          </div>
        </div>
      </div>
    </div>,
};
