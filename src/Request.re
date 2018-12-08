open Api;

let makeHeaders = (token: option(string)) => {
  let content_type = ("content-type", "application/json");
  switch (token) {
  | None => [|content_type|]
  | Some(t) => [|content_type, ("authorization", "Token " ++ t)|]
  };
};

let makeInit = (method, token, data: option(Js.Json.t)) => {
  let defaultInit =
    Bs_fetch.RequestInit.make(
      ~method_=method,
      ~headers=Bs_fetch.HeadersInit.makeWithArray @@ makeHeaders(token),
    );
  switch (data) {
  | None => defaultInit()
  | Some(d) =>
    defaultInit(~body=Bs_fetch.BodyInit.make @@ Js.Json.stringify(d), ())
  };
};

let constructUrl = url => apiUrlBase ++ mapUrl(url);

let send_ = (requestMethod, token, jsonData, actionFunc, url) => {
  open Js.Promise;
  let request = makeInit(requestMethod, token, jsonData);
  Bs_fetch.(
    fetchWithInit(url, request)
    |> then_(response =>
         Bs_fetch.Response.(actionFunc(status(response), text(response)))
         |> resolve
       )
  );
};

let registerNewUser = (registerFunc, jsonData) =>
  send_(Post, None, Some(jsonData), registerFunc, constructUrl(Register));

let authenticateUser = (loginFunc, jsonData) =>
  send_(
    Post,
    None,
    Some(jsonData),
    loginFunc,
    constructUrl(Api.Authenticate),
  );

let updateUser = (updateUserFunc, jsonData, token) =>
  send_(
    Put,
    token,
    Some(jsonData),
    updateUserFunc,
    constructUrl(Api.UpdateUser),
  );

let getCurrentUser = (getUserFunc, token) =>
  send_(Get, token, None, getUserFunc, constructUrl(Api.CurrentUser));

let getMyArticles = (getArticleFunc, name, token) => {
  let urlAfterBase = apiUrlBase ++ mapUrl(Api.Articles) ++ "?author=" ++ name;
  send_(Get, token, None, getArticleFunc, urlAfterBase);
};

let getFavoritedArticles = (articleFunc, name, token) => {
  let urlAfterBase =
    apiUrlBase ++ mapUrl(Api.Articles) ++ "?favorited=" ++ name;
  send_(Get, token, None, articleFunc, urlAfterBase);
};

let getArticlesByTag = (articleFunc, tagName, token) => {
  let urlAfterBase = apiUrlBase ++ mapUrl(Api.Articles) ++ "?tag=" ++ tagName;
  send_(Get, token, None, articleFunc, urlAfterBase);
};

let getGlobalArticles = (getArticlesFunc, token, limit, offset) => {
  let urlAfterBase =
    apiUrlBase
    ++ mapUrl(Api.Articles)
    ++ "?limit="
    ++ string_of_int(limit)
    ++ "&offset="
    ++ string_of_int(offset);
  send_(Get, token, None, getArticlesFunc, urlAfterBase);
};

let getPoplarTags = getTagsFunc =>
  send_(Get, None, None, getTagsFunc, constructUrl(Api.Tags));

let submitNewArticle = (submissionResponse, jsonData, token) =>
  send_(
    Post,
    token,
    Some(jsonData),
    submissionResponse,
    constructUrl(Api.Articles),
  );

let commentsForArticle = (slug, commentsFunc) =>
  send_(
    Get,
    None,
    None,
    commentsFunc,
    constructUrl(Api.ArticleCommentBySlug(slug)),
  );

let mutedResponse = (_, _) => ();

let deleteCommentForArticle = (slug, commentId, token) =>
  send_(
    Delete,
    token,
    None,
    mutedResponse,
    constructUrl(Api.DeleteComment(slug, commentId)),
  );

let followUser = (username, token) =>
  /* Using a muted response even though it returns a profile. It might be needed later */
  send_(
    Post,
    token,
    None,
    mutedResponse,
    constructUrl(Api.Follow(username)),
  );

let unFollowUser = (username, token) =>
  send_(
    Delete,
    token,
    None,
    mutedResponse,
    constructUrl(Api.Unfollow(username)),
  );

let getFeed = (token, articleListFunc) =>
  send_(Get, token, None, articleListFunc, constructUrl(Api.Feed));

let favoriteArticle = (token, slug) =>
  send_(
    Post,
    token,
    None,
    mutedResponse,
    constructUrl(Api.ArticleFavorite(slug)),
  );

let unfavoriteArticle = (token, slug) =>
  send_(
    Delete,
    token,
    None,
    mutedResponse,
    constructUrl(Api.ArticleFavorite(slug)),
  );
