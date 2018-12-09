let makeHeaders = (token: option(string)) => {
  let content_type = ("content-type", "application/json");
  switch (token) {
  | None => [|content_type|]
  | Some(t) => [|content_type, ("authorization", "Token " ++ t)|]
  };
};

let makeInit = (data: option(Js.Json.t), ~method_, ~token) => {
  let defaultInit =
    Bs_fetch.RequestInit.make(
      ~method_,
      ~headers=Bs_fetch.HeadersInit.makeWithArray @@ makeHeaders(token),
    );
  switch (data) {
  | None => defaultInit()
  | Some(d) =>
    defaultInit(~body=Bs_fetch.BodyInit.make @@ Js.Json.stringify(d), ())
  };
};

let send_ = (data, ~method_=Fetch.Get, ~url, ~token=None, ~f) =>
  Bs_fetch.fetchWithInit(url, makeInit(data, ~method_, ~token))
  |> Js.Promise.then_(response =>
       Bs_fetch.Response.(f(status(response), text(response)))
       ->Js.Promise.resolve
     );

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
  let all = (~limit, ~offset, ~token, ~f) =>
    get(
      ~url=
        url(Articles)
        ++ "?limit="
        ++ string_of_int(limit)
        ++ "&offset="
        ++ string_of_int(offset),
      ~token,
      ~f,
    );

  let byAuthor = (author, ~token, ~f) =>
    get(~url=url(Articles) ++ "?author=" ++ author, ~token, ~f);

  let byTag = (tagName, ~token, ~f) =>
    get(~url=url(Articles) ++ "?tag=" ++ tagName, ~token, ~f);

  let comments = (slug, ~f) =>
    get(~token=None, ~url=url(ArticleCommentBySlug(slug)), ~f);

  let deleteComment = (~id, ~slug, ~token) =>
    delete(~url=url(DeleteComment(slug, id)), ~token, ~f=Response.discard);

  let favorite = (slug, ~token) =>
    post(
      None,
      ~url=url(ArticleFavorite(slug)),
      ~token,
      ~f=Response.discard,
    );

  let favoritedBy = (name, ~token, ~f) =>
    get(~url=url(Articles) ++ "?favorited=" ++ name, ~token, ~f);

  let feed = (~token, ~f) => get(~url=url(Feed), ~token, ~f);

  let submit = (data, ~token, ~f) =>
    post(Some(data), ~url=url(Articles), ~token, ~f);

  let unfavorite = (slug, ~token) =>
    delete(~url=url(ArticleFavorite(slug)), ~token, ~f=Response.discard);
};

module Tags = {
  let all = (~f) => get(~url=url(Tags), ~f);
};

module User = {
  let current = (~token, ~f) => get(~url=url(CurrentUser), ~token, ~f);

  let follow = (username, ~token) =>
    /* Using a discard even though it returns a profile. It might be needed later */
    post(None, ~url=url(Follow(username)), ~token, ~f=Response.discard);

  let logIn = (data, ~f) => post(Some(data), ~url=url(LogIn), ~f);

  let register = (data, ~f) => post(Some(data), ~url=url(Register), ~f);

  let saveSettings = (data, ~token, ~f) =>
    put(Some(data), ~url=url(UpdateUser), ~token, ~f);

  let unFollow = (username, ~token) =>
    delete(~url=url(Unfollow(username)), ~token, ~f=Response.discard);
};
