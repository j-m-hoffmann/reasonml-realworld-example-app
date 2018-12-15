let checkForErrors = json =>
  Js.Json.decodeObject(json)
  |> Js.Option.andThen((. prop) => Js.Dict.get(prop, "errors"));

let getUserGraph = json =>
  Js.Json.decodeObject(json)
  ->Js.Option.andThen((. prop) => Js.Dict.get(prop, "user"), _);

let parseNewUser = (json): User.registered => {
  let errors =
    Json.Decode.(optional(field("errors", Errors.fromJson), json));
  switch (errors) {
  | None => {user: User.fromJson(json), errors: None}
  | _ => {user: User.empty, errors}
  };
};
