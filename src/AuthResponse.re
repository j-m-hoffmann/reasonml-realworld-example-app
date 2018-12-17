module Errors = {
  type t = {
    email: option(array(string)),
    password: option(array(string)),
    username: option(array(string)),
  };

  let fromJson = json =>
    Json.Decode.{
      email: json |> optional(field("email", array(string))),
      password: json |> optional(field("password", array(string))),
      username: json |> optional(field("username", array(string))),
    };

  let get = json =>
    Js.Json.decodeObject(json)
    |> Js.Option.andThen((. prop) => Js.Dict.get(prop, "errors"));

  let toArray = json => {
    switch (Js.Json.decodeObject(json)) {
    | Some(errors) =>
      let errorKeys = Js.Dict.keys(errors);
      let errorValues = Js.Dict.values(errors);
      Belt.Array.mapWithIndex(
        errorKeys,
        (i, errorField) => {
          let validationError = errorValues[i];
          let frontCaps = String.capitalize(errorField);
          {j|$frontCaps $validationError|j};
        },
      );
    | None => [||]
    };
  };
};

module Data = {
  type t = {
    bio: option(string),
    createdAt: string,
    email: string,
    id: int,
    image: option(string),
    token: string,
    updatedAt: string,
    username: string,
  };

  let fromJson = json =>
    Json.Decode.{
      bio: json |> optional(field("bio", string)),
      createdAt: json |> field("createdAt", string),
      email: json |> field("email", string),
      id: json |> field("id", int),
      image: json |> optional(field("image", string)),
      token: json |> field("token", string),
      updatedAt: json |> field("updatedAt", string),
      username: json |> field("username", string),
    };

  let get = json =>
    Js.Json.decodeObject(json)
    ->Js.Option.andThen((. prop) => Js.Dict.get(prop, "user"), _)
    ->Belt.Option.getWithDefault(Js.Json.parseExn({j|{}|j}));
};

type t =
  | User(Data.t)
  | Errors(array(string));

let fromJson = json => {
  switch (Errors.get(json)) {
  | None => User(Data.(get(json)->fromJson))
  | Some(e) => Errors(Errors.toArray(e))
  };
};
