type state = {
  bio: string,
  email: string,
  image: string,
  name: string,
  password: string,
};

type action =
  | SettingsFetched(state)
  | SettingsUpdated
  | UpdateBio(string)
  | UpdateEmail(string)
  | UpdateImage(string)
  | UpdateName(string)
  | UpdatePassword(string);

module Encode = {
  let userSettings = (settings: state) =>
    Json.Encode.(
      object_([
        ("email", string(settings.email)),
        ("password", string(settings.password)),
        ("image", string(settings.image)),
        ("username", string(settings.name)),
        ("bio", string(settings.bio)),
      ])
    );

  let user = (settings: state) =>
    Json.Encode.(object_([("user", userSettings(settings))]));

  let token = currentUser => Json.Encode.[("token", string(currentUser))];
};

let updateSettings = (router, event, {ReasonReact.state}) => {
  event->ReactEvent.Mouse.preventDefault;
  let responseCatch = (_status, payload) => {
    DirectorRe.setRoute(router, "/profile");
    payload
    |> Js.Promise.then_(result => {
         Js.log(result);
         result |> Js.Promise.resolve;
       })
    |> ignore;
  };
  JsonRequests.updateUser(
    responseCatch,
    Encode.user(state),
    LocalStorage.getToken(),
  )
  |> ignore;
};

let component = ReasonReact.reducerComponent("Settings");
let make = (~router, _children) => {
  ...component,
  initialState: () => {bio: "", email: "", image: "", name: "", password: ""},
  reducer: (action, state) =>
    switch (action) {
    | SettingsFetched({bio, email, image, name}) =>
      ReasonReact.Update({...state, bio, email, image, name})
    | SettingsUpdated => ReasonReact.NoUpdate
    | UpdateBio(bio) => ReasonReact.Update({...state, bio})
    | UpdateEmail(email) => ReasonReact.Update({...state, email})
    | UpdateImage(image) => ReasonReact.Update({...state, image})
    | UpdateName(name) => ReasonReact.Update({...state, name})
    | UpdatePassword(password) => ReasonReact.Update({...state, password})
    },
  didMount: self => {
    open JsonRequests;
    let reduceCurrentUser = (_status, jsonPayload) =>
      jsonPayload
      |> Js.Promise.then_(result => {
           let registered = parseNewUser(result);

           self.send(_ =>
             SettingsFetched({
               bio: Belt.Option.getWithDefault(registered.user.bio, ""),
               email: registered.user.email,
               image: Belt.Option.getWithDefault(registered.user.image, ""),
               name: registered.user.username,
               password: "",
             })
           );

           registered.user->Js.Promise.resolve;
         });

    let displayResult = result => {
      if (result == "401") {
        DirectorRe.setRoute(router, "/login");
      };

      getUserGraph(result)->User.fromJson(_).token
      ->Some
      ->getCurrentUser(reduceCurrentUser, _)
      ->ignore;

      result->Js.Promise.resolve;
    };

    let reduceUser = (_status, jsonPayload) =>
      jsonPayload |> Js.Promise.then_(displayResult);

    getCurrentUser(reduceUser, LocalStorage.getToken())->ignore;
  },
  render: ({state, send, handle}) =>
    <div className="settings-page">
      <div className="container page">
        <div className="row">
          <div className="col-md-6 offset-md-3 col-xs-12">
            <h1 className="text-xs-center">
              {ReasonReact.string("Your Settings")}
            </h1>
            <form>
              <fieldset>
                <fieldset className="form-group">
                  <input
                    className="form-control"
                    type_="text"
                    placeholder="URL of profile picture"
                    value={state.image}
                    onChange={
                      event =>
                        send(
                          UpdateImage(ReactEvent.Form.target(event)##value),
                        )
                    }
                  />
                </fieldset>
                <fieldset className="form-group">
                  <input
                    className="form-control form-control-lg"
                    type_="text"
                    placeholder="Your Name"
                    value={state.name}
                    onChange={
                      event =>
                        send(
                          UpdateName(ReactEvent.Form.target(event)##value),
                        )
                    }
                  />
                </fieldset>
                <fieldset className="form-group">
                  <textarea
                    className="form-control form-control-lg"
                    rows=8
                    placeholder="Short bio about you"
                    value={state.bio}
                    onChange={
                      event =>
                        send(
                          UpdateBio(ReactEvent.Form.target(event)##value),
                        )
                    }
                  />
                </fieldset>
                <fieldset className="form-group">
                  <input
                    className="form-control form-control-lg"
                    type_="text"
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
                    className="form-control form-control-lg"
                    type_="password"
                    placeholder="Password"
                    value={state.password}
                    onChange={
                      event =>
                        send(
                          UpdatePassword(
                            ReactEvent.Form.target(event)##value,
                          ),
                        )
                    }
                  />
                </fieldset>
                <button
                  className="btn btn-lg btn-primary pull-xs-right"
                  onClick={handle(updateSettings(router))}>
                  {ReasonReact.string("Update Settings")}
                </button>
              </fieldset>
            </form>
          </div>
        </div>
      </div>
    </div>,
};
