type service =
  | ArticleBySlug(string)
  | ArticleBySlugId(string, string)
  | ArticleCommentBySlug(string)
  | ArticleFavorite(string)
  | ArticleUnfavorite(string)
  | Articles
  | CurrentUser
  | DeleteComment(string, int)
  | Feed
  | Follow(string)
  | LogIn
  | Profile(string)
  | Register
  | Tags
  | Unfollow(string)
  | UpdateUser;

let toUrl = s =>
  switch (s) {
  | ArticleBySlug(slug) => "/articles/" ++ slug
  | ArticleBySlugId(slug, id) => "/articles/" ++ slug ++ "/comments/" ++ id
  | ArticleCommentBySlug(slug) => "/articles/" ++ slug ++ "/comments"
  | ArticleFavorite(slug) => "/articles/" ++ slug ++ "/favorite"
  | ArticleUnfavorite(slug) => "/articles/" ++ slug ++ "/favorite"
  | Articles => "/articles"
  | CurrentUser => "/user"
  | DeleteComment(slug, id) =>
    "/articles/" ++ slug ++ "/comments/" ++ string_of_int(id)
  | Feed => "/articles/feed"
  | Follow(username) => "/profiles/" ++ username ++ "/follow"
  | LogIn => "/users/login"
  | Profile(username) => "/profiles/" ++ username
  | Register => "/users"
  | Tags => "/tags"
  | Unfollow(username) => "/profiles/" ++ username ++ "/follow"
  | UpdateUser => "/user"
  };

let root = "https://conduit.productionready.io/api";

let url = s => root ++ toUrl(s);
