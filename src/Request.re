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

let registerNewUser = (registerFunc, jsonData) =>
  send_(Post, None, Some(jsonData), registerFunc, getUrl(Register));

let authenticateUser = (loginFunc, jsonData) =>
  send_(Post, None, Some(jsonData), loginFunc, getUrl(Authenticate));

let updateUser = (updateUserFunc, jsonData, token) =>
  send_(Put, token, Some(jsonData), updateUserFunc, getUrl(UpdateUser));

let getCurrentUser = (getUserFunc, token) =>
  send_(Get, token, None, getUserFunc, getUrl(CurrentUser));

let getMyArticles = (getArticleFunc, name, token) => {
  let urlAfterBase = getUrl(Articles) ++ "?author=" ++ name;
  send_(Get, token, None, getArticleFunc, urlAfterBase);
};

let getFavoritedArticles = (articleFunc, name, token) => {
  let urlAfterBase = getUrl(Articles) ++ "?favorited=" ++ name;
  send_(Get, token, None, articleFunc, urlAfterBase);
};

let getArticlesByTag = (articleFunc, tagName, token) => {
  let urlAfterBase = getUrl(Articles) ++ "?tag=" ++ tagName;
  send_(Get, token, None, articleFunc, urlAfterBase);
};

let getGlobalArticles = (getArticlesFunc, token, limit, offset) => {
  let urlAfterBase =
    getUrl(Articles)
    ++ "?limit="
    ++ string_of_int(limit)
    ++ "&offset="
    ++ string_of_int(offset);
  send_(Get, token, None, getArticlesFunc, urlAfterBase);
};

let getPoplarTags = getTagsFunc =>
  send_(Get, None, None, getTagsFunc, getUrl(Tags));

let submitNewArticle = (submissionResponse, jsonData, token) =>
  send_(Post, token, Some(jsonData), submissionResponse, getUrl(Articles));

let commentsForArticle = (slug, commentsFunc) =>
  send_(Get, None, None, commentsFunc, getUrl(ArticleCommentBySlug(slug)));

let mutedResponse = (_, _) => ();

let deleteCommentForArticle = (slug, commentId, token) =>
  send_(
    Delete,
    token,
    None,
    mutedResponse,
    getUrl(DeleteComment(slug, commentId)),
  );

let followUser = (username, token) =>
  /* Using a muted response even though it returns a profile. It might be needed later */
  send_(Post, token, None, mutedResponse, getUrl(Follow(username)));

let unFollowUser = (username, token) =>
  send_(Delete, token, None, mutedResponse, getUrl(Unfollow(username)));

let getFeed = (token, articleListFunc) =>
  send_(Get, token, None, articleListFunc, getUrl(Feed));

let favoriteArticle = (token, slug) =>
  send_(Post, token, None, mutedResponse, getUrl(ArticleFavorite(slug)));

let unfavoriteArticle = (token, slug) =>
  send_(Delete, token, None, mutedResponse, getUrl(ArticleFavorite(slug)));
