let checkForErrors = response =>
  Js.Json.parseExn(response)->Js.Json.decodeObject
  |> Js.Option.andThen((. prop) => Js.Dict.get(prop, "errors"));

let getUserGraph = response =>
  Js.Json.parseExn(response)
  ->Js.Json.decodeObject
  ->Js.Option.andThen((. prop) => Js.Dict.get(prop, "user"), _)
  ->Belt.Option.getWithDefault(Js.Json.parseExn({j|{}|j}));
