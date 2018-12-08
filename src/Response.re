let checkForErrors = response =>
  Js.Json.parseExn(response)->Js.Json.decodeObject
  |> Js.Option.andThen((. prop) => Js.Dict.get(prop, "errors"));

let getUserGraph = response =>
  Js.Json.parseExn(response)
  ->Js.Json.decodeObject
  ->Js.Option.andThen((. prop) => Js.Dict.get(prop, "user"), _)
  ->Belt.Option.getWithDefault(Js.Json.parseExn({j|{}|j}));

let parseNewUser = (response: string): User.registered => {
  let json = Js.Json.parseExn(response);
  let errors =
    Json.Decode.(optional(field("errors", Errors.fromJson), json));
  switch (errors) {
  | None => {user: User.fromJson(json), errors: None}
  | _ => {user: User.empty, errors}
  };
};

let discard = (_status, _response) => ();
