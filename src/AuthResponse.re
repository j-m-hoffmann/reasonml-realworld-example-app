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

  /*let toList = ({email, password, username}) =>*/
  /*switch (email, password, username) {*/
  /*| (None, None, None) => []*/
  /*};*/

  let toList = errorJson => {
    let decodedJson = Js.Json.decodeObject(errorJson);
    switch (decodedJson) {
    | Some(errorList) =>
      let errorKeys = Js.Dict.keys(errorList);
      let errorValues = Js.Dict.values(errorList);
      Array.mapi(
        (acc, errorField) => {
          let validationError = errorValues[acc];
          let frontCaps = String.capitalize(errorField);
          {j|$frontCaps $validationError|j};
        },
        errorKeys,
      )
      |> Array.to_list;
    | None => []
    };
  };
};

module User = {
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

  let empty = {
    bio: None,
    createdAt: "",
    email: "",
    id: 0,
    image: None,
    token: "",
    updatedAt: "",
    username: "",
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
};

type t = {
  errors: option(Errors.t),
  user: User.t,
};

let checkForErrors = response =>
  Js.Json.decodeObject(response)
  |> Js.Option.andThen((. prop) => Js.Dict.get(prop, "errors"));

let getUserGraph = response =>
  Js.Json.decodeObject(response)
  ->Js.Option.andThen((. prop) => Js.Dict.get(prop, "user"), _)
  ->Belt.Option.getWithDefault(Js.Json.parseExn({j|{}|j}));

let parseNewUser = json => {
  let errors =
    Json.Decode.(optional(field("errors", Errors.fromJson), json));
  switch (errors) {
  | None => {user: User.fromJson(json), errors: None}
  | _ => {user: User.empty, errors}
  };
};
