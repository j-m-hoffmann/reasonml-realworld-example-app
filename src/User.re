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

type registered = {
  errors: option(Errors.t),
  user: t,
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

let errorList = ({errors}: registered) => {
  let formatError = (fieldName, value) => {
    let prependFieldName = (fieldName, fieldValue) =>
      fieldName ++ " " ++ fieldValue;
    switch (value) {
    | Some(message) =>
      Array.map(
        errorMessage => prependFieldName(fieldName, errorMessage),
        message,
      )
    | None => [|""|]
    };
  };

  switch (errors) {
  | Some(errors) =>
    (formatError("Email", errors.email) |> Array.to_list)
    @ (formatError("Username", errors.username) |> Array.to_list)
    @ (formatError("Password", errors.password) |> Array.to_list)
  | None => []
  };
};
