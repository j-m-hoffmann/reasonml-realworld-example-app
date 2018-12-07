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
