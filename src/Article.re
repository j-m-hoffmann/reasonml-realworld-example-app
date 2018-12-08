type t = {
  slug: string,
  title: string,
  description: string,
  body: string,
  tagList: array(string),
  createdAt: string,
  updatedAt: string,
  favorited: bool,
  favoritesCount: int,
  author: Author.t,
};

let fromJson = json =>
  Json.Decode.{
    slug: json |> field("slug", string),
    title: json |> field("title", string),
    description: json |> field("description", string),
    body: json |> field("body", string),
    tagList: [||],
    createdAt: json |> field("createdAt", string),
    updatedAt: json |> field("updatedAt", string),
    favorited: json |> field("favorited", bool),
    favoritesCount: json |> field("favoritesCount", int),
    author: json |> field("author", Author.fromJson),
  };

let empty = {
  slug: "",
  title: "",
  description: "",
  body: "",
  tagList: [||],
  createdAt: "",
  updatedAt: "",
  favorited: false,
  favoritesCount: 0,
  author: Author.none,
};

type commentList = {comments: list(Comment.t)};

type state = {
  slug: string,
  commentList: list(Comment.t),
  articleBody: string,
  isFollowing: bool,
};

type action =
  | AddComment
  | DeleteComment(int)
  | FollowUser(string)
  | UnFollowUser(string)
  | FetchComments(list(Comment.t));

let deleteCommentRequest = (id, slug) =>
  Request.deleteCommentForArticle(~id, ~slug, ~token=LocalStorage.getToken())
  |> ignore;

let followUserRequest = username =>
  Request.followUser(username, ~token=LocalStorage.getToken()) |> ignore;

let unFollowUserRequest = username =>
  Request.unFollowUser(username, ~token=LocalStorage.getToken()) |> ignore;

let followUser = (isFollowing, event) =>
  isFollowing ?
    UnFollowUser(ReactEvent.Mouse.target(event)##value) :
    FollowUser(ReactEvent.Mouse.target(event)##value);

/* Add markdown parser to display properly */
let dangerousHtml: string => Js.t('a) = html => {"__html": html};

let component = ReasonReact.reducerComponent("Article");

/* let make = (~router, ~article, _children) => { */
let make = (~article, _children) => {
  ...component,
  initialState: () => {
    slug: article.slug,
    commentList: [],
    articleBody: article.body,
    isFollowing: article.author.following,
  },
  reducer: (action, state) =>
    switch (action) {
    | AddComment => ReasonReact.NoUpdate
    | DeleteComment(id) =>
      let commentsWithout =
        Belt.List.keep(state.commentList, comment => comment.id != id);
      ReasonReact.UpdateWithSideEffects(
        {...state, commentList: commentsWithout},
        (_self => deleteCommentRequest(id, state.slug)),
      );
    | FetchComments(commentList) =>
      ReasonReact.Update({...state, commentList})
    | FollowUser(username) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, isFollowing: true},
        (_self => followUserRequest(username)),
      )
    | UnFollowUser(username) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, isFollowing: false},
        (_self => unFollowUserRequest(username)),
      )
    },
  didMount: self =>
    Request.commentsForArticle(self.state.slug, ~f=(_status, payload) =>
      payload
      |> Js.Promise.then_(result => {
           let commentList =
             Json.Decode.{
               comments:
                 Js.Json.parseExn(result)
                 |> field("comments", list(Comment.fromJson)),
             };
           self.send(FetchComments(commentList.comments))
           |> Js.Promise.resolve;
           /*self.send(FetchComments(commentList.comments));*/
           /*result |> Js.Promise.resolve;*/
         })
    )
    |> ignore,
  render: ({state, send}) =>
    <div className="article-page">
      <div className="banner">
        <div className="container">
          <h1> {ReasonReact.string(article.title)} </h1>
          <div className="article-meta">
            <a href="">
              <img
                src={Belt.Option.getWithDefault(article.author.image, "")}
              />
            </a>
            <div className="info">
              <a href="" className="author">
                {ReasonReact.string(article.author.username)}
              </a>
              <span className="date">
                Js.Date.(fromString(article.createdAt)->toDateString)
                ->ReasonReact.string
              </span>
            </div>
            <button
              className="btn btn-sm btn-outline-secondary"
              value={article.author.username}
              onClick={e => send(followUser(state.isFollowing, e))}>
              <i className="ion-plus-round" />
              {ReasonReact.string(" ")}
              {
                ReasonReact.string(
                  (state.isFollowing ? "unfollow " : "follow ")
                  ++ article.author.username,
                )
              }
              <span className="counter"> {ReasonReact.string("(10)")} </span>
            </button>
            {ReasonReact.string("  ")}
            <button className="btn btn-sm btn-outline-primary">
              <i className="ion-heart" />
              {ReasonReact.string(" ")}
              {ReasonReact.string("Favorite Post")}
              <span className="counter"> {ReasonReact.string("(0)")} </span>
            </button>
          </div>
        </div>
      </div>
      <div className="container page">
        <div className="row article-content">
          <div className="col-md-12">
            <div dangerouslySetInnerHTML={dangerousHtml(article.body)} />
          </div>
        </div>
        <hr />
        <div className="article-actions">
          <div className="article-meta">
            <a href="profile.html">
              <img
                src={Belt.Option.getWithDefault(article.author.image, "")}
              />
            </a>
            <div className="info">
              <a href="" className="author">
                {ReasonReact.string(article.author.username)}
              </a>
              <span className="date">
                {
                  ReasonReact.string(
                    Js.Date.fromString(article.createdAt)
                    |> Js.Date.toDateString,
                  )
                }
              </span>
            </div>
            <button
              className="btn btn-sm btn-outline-secondary"
              value={article.author.username}
              onClick={e => send(followUser(state.isFollowing, e))}>
              <i className="ion-plus-round" />
              {ReasonReact.string(" ")}
              {
                ReasonReact.string(
                  (state.isFollowing ? "unfollow " : "follow ")
                  ++ article.author.username,
                )
              }
              <span className="counter"> {ReasonReact.string("(0)")} </span>
            </button>
            {ReasonReact.string(" ")}
            <button className="btn btn-sm btn-outline-primary">
              <i className="ion-heart" />
              {ReasonReact.string(" ")}
              {ReasonReact.string("Favorite Post")}
              <span className="counter"> {ReasonReact.string("(0)")} </span>
            </button>
          </div>
        </div>
        <div className="row">
          <div className="col-xs-12 col-md-8 offset-md-2">
            <form className="card comment-form">
              <div className="card-block">
                <textarea
                  className="form-control"
                  placeholder="Write a comment..."
                  rows=3
                />
              </div>
              <div className="card-footer">
                <img
                  src="http://i.imgur.com/Qr71crq.jpg"
                  className="comment-author-img"
                />
                <button className="btn btn-sm btn-primary">
                  {ReasonReact.string("Post Comment")}
                </button>
              </div>
            </form>
            {
              Belt.List.toArray(state.commentList)
              ->Belt.Array.mapWithIndex(_, (index, comment) =>
                  <Comment
                    comment
                    index
                    deleteComment={_ => send(DeleteComment(comment.id))}
                  />
                )
              ->ReasonReact.array
            }
          </div>
        </div>
      </div>
    </div>,
};
