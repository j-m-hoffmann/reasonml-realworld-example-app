type t = {
  username: string,
  bio: option(string),
  image: option(string),
  following: bool,
};

let fromJson = json =>
  Json.Decode.{
    username: json |> field("username", string),
    bio: json |> optional(field("bio", string)),
    image: json |> optional(field("image", string)),
    following: json |> field("following", bool),
  };
