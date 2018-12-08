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
