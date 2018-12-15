open Dom.Storage;

let bio = () => getItem("bio", localStorage)->Belt.Option.getWithDefault("");

let image = () =>
  getItem("image", localStorage)->Belt.Option.getWithDefault("");

let token = () => getItem("jwt", localStorage);

let username = () =>
  getItem("username", localStorage)->Belt.Option.getWithDefault("anonymous");

let save = (user: AuthResponse.User.t) => {
  switch (user.bio) {
  | Some(bio) => setItem("bio", bio, localStorage)
  | None => ()
  };
  switch (user.image) {
  | Some(image) => setItem("image", image, localStorage)
  | None => ()
  };
  setItem("jwt", user.token, localStorage);
  setItem("username", user.username, localStorage);
};
