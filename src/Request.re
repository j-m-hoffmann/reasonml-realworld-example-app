let makeHeaders = (token: option(string)) => {
  let content_type = ("content-type", "application/json");
  switch (token) {
  | None => [|content_type|]
  | Some(t) => [|content_type, ("authorization", "Token " ++ t)|]
  };
};

let makeInit = (data: option(Js.Json.t), ~method_, ~token) => {
  let defaultInit =
    Fetch.RequestInit.make(
      ~method_,
      ~headers=makeHeaders(token)->Fetch.HeadersInit.makeWithArray,
    );
  switch (data) {
  | None => defaultInit()
  | Some(d) =>
    defaultInit(~body=Js.Json.stringify(d)->Fetch.BodyInit.make, ())
  };
};

let handle = (data, ~method_, ~url, ~token, ~f) => {
  let request = makeInit(data, ~token, ~method_);
  Js.Promise.(
    Fetch.fetchWithInit(url, request)
    |> then_(Fetch.Response.text)
    |> then_(body =>
         (
           switch (Json.parse(body)) {
           | Some(json) => f(json)
           | None => Js.Console.error("Error parsing JSON")
           }
         )
         |> resolve
       )
  );
};

let delete = (~url, ~token, ~f) =>
  handle(None, ~method_=Fetch.Delete, ~token, ~url, ~f);

let get = (~url, ~token, ~f) =>
  handle(None, ~method_=Fetch.Get, ~token, ~url, ~f);

let post = (data, ~url, ~token, ~f) =>
  handle(data, ~method_=Fetch.Post, ~token, ~url, ~f);

let put = (data, ~url, ~token, ~f) =>
  handle(data, ~method_=Fetch.Put, ~token, ~url, ~f);

open Api;

let discard = _response => ();

module Article = {
  let limit = (count, page) =>
    "?limit="
    ++ string_of_int(count)
    ++ "&offset="
    ++ string_of_int(page * count);

  let all = (~page, ~f) =>
    get(
      ~url=url(Articles) ++ limit(10, page),
      ~token=LocalStorage.token(),
      ~f,
    );

  let byAuthor = (author, ~f) =>
    get(
      ~url=url(Articles) ++ "?author=" ++ author,
      ~token=LocalStorage.token(),
      ~f,
    );

  let byTag = (name, ~f) =>
    get(
      ~url=url(Articles) ++ "?tag=" ++ name,
      ~token=LocalStorage.token(),
      ~f,
    );

  let comments = (slug, ~f) =>
    get(~token=None, ~url=url(ArticleCommentBySlug(slug)), ~f);

  let deleteComment = (~id, ~slug) =>
    delete(
      ~url=url(DeleteComment(slug, id)),
      ~token=LocalStorage.token(),
      ~f=discard,
    );

  let favorite = slug =>
    post(
      None,
      ~url=url(ArticleFavorite(slug)),
      ~token=LocalStorage.token(),
      ~f=discard,
    );

  let favoritedBy = (name, ~f) =>
    get(
      ~url=url(Articles) ++ "?favorited=" ++ name,
      ~token=LocalStorage.token(),
      ~f,
    );

  let feed = (~f) =>
    get(~url=url(Feed) ++ limit(10, 0), ~token=LocalStorage.token(), ~f);

  let submit = (data, ~f) =>
    post(Some(data), ~url=url(Articles), ~token=LocalStorage.token(), ~f);

  let unfavorite = slug =>
    delete(
      ~url=url(ArticleFavorite(slug)),
      ~token=LocalStorage.token(),
      ~f=discard,
    );
};

module Tags = {
  let all = (~f) => get(~url=url(Tags), ~token=None, ~f);
};

module User = {
  let current = (~token=LocalStorage.token(), ~f) =>
    get(~url=url(CurrentUser), ~token, ~f);

  let follow = username =>
    /* Using a discard even though it returns a profile. It might be needed later */
    post(
      None,
      ~url=url(Follow(username)),
      ~token=LocalStorage.token(),
      ~f=discard,
    );

  let logIn = (data, ~f) =>
    post(Some(data), ~token=None, ~url=url(LogIn), ~f);

  let register = (data, ~f) =>
    post(Some(data), ~token=None, ~url=url(Register), ~f);

  let saveSettings = (data, ~f) =>
    put(Some(data), ~token=LocalStorage.token(), ~url=url(UpdateUser), ~f);

  let unFollow = username =>
    delete(
      ~url=url(Unfollow(username)),
      ~token=LocalStorage.token(),
      ~f=discard,
    );
};
