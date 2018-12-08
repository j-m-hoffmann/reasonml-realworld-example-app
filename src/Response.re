let checkForErrors = response =>
  Js.Json.parseExn(response)->Js.Json.decodeObject
  |> Js.Option.andThen((. prop) => Js.Dict.get(prop, "errors"));
