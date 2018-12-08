open Dom.Storage;

let getToken = () => getItem("jwt", localStorage);

let getUser = () => (
  getItem("bio", localStorage),
  getItem("image", localStorage),
  getItem("username", localStorage),
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
