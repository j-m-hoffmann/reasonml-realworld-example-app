open Dom.Storage;

let getToken = () => getItem("jwt", localStorage);

let getUser = () => (
  getItem("username", localStorage),
  getItem("bio", localStorage),
  getItem("image", localStorage),
);

let saveToken = value => setItem("jwt", value, localStorage);

let saveUser = (bio, image, username) => {
  switch (bio) {
  | Some(bio) => setItem("bio", bio, localStorage)
  | None => ()
  };
  switch (image) {
  | Some(image) => setItem("image", image, localStorage)
  | None => ()
  };
  setItem("username", username, localStorage);
};
