open Dom.Storage;

let getToken = () => getItem("jwt", localStorage);

let getUser = () => (
  getItem("username", localStorage),
  getItem("bio", localStorage),
  getItem("image", localStorage),
);

let saveToken = value => setItem("jwt", value, localStorage);

let saveUser = (username, bio, image) => {
  setItem("username", username, localStorage);

  switch (image) {
  | Some(image) => setItem("image", image, localStorage)
  | None => ()
  };

  switch (bio) {
  | Some(bio) => setItem("bio", bio, localStorage)
  | None => ()
  };
};
