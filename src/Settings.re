type state = {
  bio: string,
  email: string,
  image: string,
  name: string,
  password: string,
};

type action =
  | LogOut
  | SettingsFetched(state)
  | UpdateBio(string)
  | UpdateEmail(string)
  | UpdateImage(string)
  | UpdateName(string)
  | UpdatePassword(string)
  | UpdateSettings;

let toJson = settings =>
  Json.Encode.(
    object_([
      (
        "user",
        object_([
          ("email", string(settings.email)),
          ("password", string(settings.password)),
          ("image", string(settings.image)),
          ("username", string(settings.name)),
          ("bio", string(settings.bio)),
        ]),
      ),
    ])
  );

let component = ReasonReact.reducerComponent("Settings");

let make = (~logOut, ~router, _children) => {
  ...component,
  initialState: () => {bio: "", email: "", image: "", name: "", password: ""},
  reducer: (action, state) =>
    switch (action) {
    | LogOut => ReasonReact.SideEffects(_ => logOut())
    | SettingsFetched(settings) => ReasonReact.Update(settings)
    | UpdateBio(bio) => ReasonReact.Update({...state, bio})
    | UpdateEmail(email) => ReasonReact.Update({...state, email})
    | UpdateImage(image) => ReasonReact.Update({...state, image})
    | UpdateName(name) => ReasonReact.Update({...state, name})
    | UpdatePassword(password) => ReasonReact.Update({...state, password})
    | UpdateSettings =>
      ReasonReact.SideEffects(
        _ =>
          Request.User.saveSettings(
            toJson(state),
            ~f=json => {
              Json.stringify(json)->Js.log;
              DirectorRe.setRoute(router, "/profile");
            },
          )
          |> ignore,
      )
    },
  didMount: self =>
    switch (LocalStorage.token()) {
    | Some(_) as token =>
      Request.User.current(~token, ~f=json =>
        switch (AuthResponse.fromJson(json)) {
        | User(user) =>
          self.send(
            SettingsFetched({
              bio: Belt.Option.getWithDefault(user.bio, ""),
              email: user.email,
              image: Belt.Option.getWithDefault(user.image, ""),
              name: user.username,
              password: "",
            }),
          )
        | Errors(_) =>
          self.send(
            SettingsFetched({
              bio: "Fetching settings failed",
              email: "",
              image: "",
              name: "",
              password: "",
            }),
          )
        }
      )
      |> ignore
    | None => DirectorRe.setRoute(router, "/login")
    },
  render: ({state, send}) =>
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
                    onChange={event =>
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
                    onChange={event =>
                      send(UpdateName(ReactEvent.Form.target(event)##value))
                    }
                  />
                </fieldset>
                <fieldset className="form-group">
                  <textarea
                    className="form-control form-control-lg"
                    rows=8
                    placeholder="Short bio about you"
                    value={state.bio}
                    onChange={event =>
                      send(UpdateBio(ReactEvent.Form.target(event)##value))
                    }
                  />
                </fieldset>
                <fieldset className="form-group">
                  <input
                    className="form-control form-control-lg"
                    type_="text"
                    placeholder="Email"
                    value={state.email}
                    onChange={event =>
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
                    onChange={event =>
                      send(
                        UpdatePassword(ReactEvent.Form.target(event)##value),
                      )
                    }
                  />
                </fieldset>
                <button
                  className="btn btn-lg btn-primary pull-xs-right"
                  onClick={event => {
                    event->ReactEvent.Mouse.preventDefault;
                    send(UpdateSettings);
                  }}>
                  {ReasonReact.string("Update Settings")}
                </button>
                <button
                  className="btn btn-lg btn-secondary pull-xs-left"
                  onClick={event => {
                    event->ReactEvent.Mouse.preventDefault;
                    send(LogOut);
                  }}>
                  {ReasonReact.string("Log Out")}
                </button>
              </fieldset>
            </form>
          </div>
        </div>
      </div>
    </div>,
};
