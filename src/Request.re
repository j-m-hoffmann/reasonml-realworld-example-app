open Api;

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

let registerNewUser = (data, ~f) =>
  send_(Some(data), ~url=getUrl(Register), ~method_=Post, ~f);

let authenticateUser = (data, ~f) =>
  send_(Some(data), ~method_=Post, ~url=getUrl(Authenticate), ~f);

let updateUser = (data, ~token, ~f) =>
  send_(Some(data), ~method_=Put, ~url=getUrl(UpdateUser), ~token, ~f);

let getCurrentUser = (~token, ~f) =>
  send_(None, ~url=getUrl(CurrentUser), ~token, ~f);

let getMyArticles = (name, ~token, ~f) =>
  send_(None, ~url=getUrl(Articles) ++ "?author=" ++ name, ~token, ~f);

let getFavoritedArticles = (name, ~token, ~f) =>
  send_(None, ~url=getUrl(Articles) ++ "?favorited=" ++ name, ~token, ~f);

let getArticlesByTag = (tagName, ~token, ~f) =>
  send_(None, ~url=getUrl(Articles) ++ "?tag=" ++ tagName, ~token, ~f);

let getGlobalArticles = (~limit, ~offset, ~token, ~f) =>
  send_(
    None,
    ~url=
      getUrl(Articles)
      ++ "?limit="
      ++ string_of_int(limit)
      ++ "&offset="
      ++ string_of_int(offset),
    ~token,
    ~f,
  );

let getPopularTags = (~f) => send_(None, ~url=getUrl(Tags), ~f);

let submitNewArticle = (data, ~token, ~f) =>
  send_(Some(data), ~method_=Post, ~url=getUrl(Articles), ~token, ~f);

let commentsForArticle = (slug, ~f) =>
  send_(None, ~url=getUrl(ArticleCommentBySlug(slug)), ~f);

let deleteCommentForArticle = (~id, ~slug, ~token) =>
  send_(
    None,
    ~method_=Delete,
    ~url=getUrl(DeleteComment(slug, id)),
    ~token,
    ~f=Response.discard,
  );

/* Using a muted response even though it returns a profile. It might be needed later */
let followUser = (username, ~token) =>
  send_(
    None,
    ~method_=Post,
    ~url=getUrl(Follow(username)),
    ~token,
    ~f=Response.discard,
  );

let unFollowUser = (username, ~token) =>
  send_(
    None,
    ~method_=Delete,
    ~url=getUrl(Unfollow(username)),
    ~token,
    ~f=Response.discard,
  );

let getFeed = (~token, ~f) => send_(None, ~url=getUrl(Feed), ~token, ~f);

let favoriteArticle = (slug, ~token) =>
  send_(
    None,
    ~method_=Post,
    ~url=getUrl(ArticleFavorite(slug)),
    ~token,
    ~f=Response.discard,
  );

let unfavoriteArticle = (slug, ~token) =>
  send_(
    None,
    ~method_=Delete,
    ~url=getUrl(ArticleFavorite(slug)),
    ~token,
    ~f=Response.discard,
  );
