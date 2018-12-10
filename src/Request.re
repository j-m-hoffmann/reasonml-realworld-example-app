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
      ~headers=Fetch.HeadersInit.makeWithArray @@ makeHeaders(token),
    );
  switch (data) {
  | None => defaultInit()
  | Some(d) =>
    defaultInit(~body=Fetch.BodyInit.make @@ Js.Json.stringify(d), ())
  };
};

let send_ = (data, ~method_, ~url, ~token, ~f) => {
  let request = makeInit(data, ~token, ~method_);
  Js.Promise.(
    Fetch.fetchWithInit(url, request)
    |> then_(response =>
         f(Fetch.Response.status(response), Fetch.Response.text(response))
         |> resolve
       )
  );
};

let delete = (~url, ~token, ~f) =>
  send_(None, ~method_=Fetch.Delete, ~token, ~url, ~f);

let get = (~url, ~token, ~f) =>
  send_(None, ~method_=Fetch.Get, ~token, ~url, ~f);

let post = (data, ~url, ~token, ~f) =>
  send_(data, ~method_=Fetch.Post, ~token, ~url, ~f);

let put = (data, ~url, ~token, ~f) =>
  send_(data, ~method_=Fetch.Put, ~token, ~url, ~f);

open Api;

module Article = {
  let limit = (count, page) =>
    "?limit="
    ++ string_of_int(count)
    ++ "&offset="
    ++ string_of_int(page * count);

  let all = (~page, ~f) =>
    get(
      ~url=url(Articles) ++ limit(10, page),
      ~token=LocalStorage.getToken(),
      ~f,
    );

  let byAuthor = (author, ~f) =>
    get(
      ~url=url(Articles) ++ "?author=" ++ author,
      ~token=LocalStorage.getToken(),
      ~f,
    );

  let byTag = (tagName, ~f) =>
    get(
      ~url=url(Articles) ++ "?tag=" ++ tagName,
      ~token=LocalStorage.getToken(),
      ~f,
    );

  let comments = (slug, ~f) =>
    get(~token=None, ~url=url(ArticleCommentBySlug(slug)), ~f);

  let deleteComment = (~id, ~slug) =>
    delete(
      ~url=url(DeleteComment(slug, id)),
      ~token=LocalStorage.getToken(),
      ~f=Response.discard,
    );

  let favorite = slug =>
    post(
      None,
      ~url=url(ArticleFavorite(slug)),
      ~token=LocalStorage.getToken(),
      ~f=Response.discard,
    );

  let favoritedBy = (name, ~f) =>
    get(
      ~url=url(Articles) ++ "?favorited=" ++ name,
      ~token=LocalStorage.getToken(),
      ~f,
    );

  let feed = (~f) =>
    get(~url=url(Feed) ++ limit(10, 0), ~token=LocalStorage.getToken(), ~f);

  let submit = (data, ~f) =>
    post(
      Some(data),
      ~url=url(Articles),
      ~token=LocalStorage.getToken(),
      ~f,
    );

  let unfavorite = slug =>
    delete(
      ~url=url(ArticleFavorite(slug)),
      ~token=LocalStorage.getToken(),
      ~f=Response.discard,
    );
};

module Tags = {
  let all = (~f) => get(~url=url(Tags), ~token=None, ~f);
};

module User = {
  let current = (~token=LocalStorage.getToken(), ~f) =>
    get(~url=url(CurrentUser), ~token, ~f);

  let follow = username =>
    /* Using a discard even though it returns a profile. It might be needed later */
    post(
      None,
      ~url=url(Follow(username)),
      ~token=LocalStorage.getToken(),
      ~f=Response.discard,
    );

  let logIn = (data, ~f) =>
    post(Some(data), ~token=None, ~url=url(LogIn), ~f);

  let register = (data, ~f) =>
    post(Some(data), ~token=None, ~url=url(Register), ~f);

  let saveSettings = (data, ~f) =>
    put(
      Some(data),
      ~token=LocalStorage.getToken(),
      ~url=url(UpdateUser),
      ~f,
    );

  let unFollow = username =>
    delete(
      ~url=url(Unfollow(username)),
      ~token=LocalStorage.getToken(),
      ~f=Response.discard,
    );
};
