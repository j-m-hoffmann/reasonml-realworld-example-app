type state = {
  bio: string,
  email: string,
  image: string,
  name: string,
  password: string,
};

type action =
  | GoToProfile
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

let make = (~router, _children) => {
  ...component,
  initialState: () => {bio: "", email: "", image: "", name: "", password: ""},
  reducer: (action, state) =>
    switch (action) {
    | GoToProfile =>
      ReasonReact.SideEffects((_ => DirectorRe.setRoute(router, "/profile")))
    | SettingsFetched(settings) => ReasonReact.Update(settings)
    | UpdateBio(bio) => ReasonReact.Update({...state, bio})
    | UpdateEmail(email) => ReasonReact.Update({...state, email})
    | UpdateImage(image) => ReasonReact.Update({...state, image})
    | UpdateName(name) => ReasonReact.Update({...state, name})
    | UpdatePassword(password) => ReasonReact.Update({...state, password})
    | UpdateSettings =>
      ReasonReact.SideEffects(
        (
          self =>
            Request.User.saveSettings(toJson(state), ~f=(_status, payload) =>
              payload
              |> Js.Promise.then_(result => {
                   Js.log(result);
                   self.send(GoToProfile);
                   result |> Js.Promise.resolve;
                 })
              |> ignore
            )
            |> ignore
        ),
      )
    },
  didMount: self =>
    Request.User.current(~f=(status, payload) =>
      payload
      |> Js.Promise.then_(result => {
           if (status == 401) {
             DirectorRe.setRoute(router, "/login");
           } else {
             /*TODO  check again*/
             Response.getUserGraph(result)->User.fromJson(_).token
             ->Some
             ->Request.User.current(~token=_, ~f=(_status, payload) =>
                 payload
                 |> Js.Promise.then_(result => {
                      let registered = Response.parseNewUser(result);

                      self.send(
                        SettingsFetched({
                          bio:
                            Belt.Option.getWithDefault(
                              registered.user.bio,
                              "",
                            ),
                          email: registered.user.email,
                          image:
                            Belt.Option.getWithDefault(
                              registered.user.image,
                              "",
                            ),
                          name: registered.user.username,
                          password: "",
                        }),
                      );

                      registered.user |> Js.Promise.resolve;
                    })
               )
             |> ignore;
           };
           result |> Js.Promise.resolve;
         })
    )
    |> ignore,

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
                  onClick={
                    event => {
                      event->ReactEvent.Mouse.preventDefault;
                      send(UpdateSettings);
                    }
                  }>
                  {ReasonReact.string("Update Settings")}
                </button>
              </fieldset>
            </form>
          </div>
        </div>
      </div>
    </div>,
};
