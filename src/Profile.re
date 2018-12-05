open Models;

type state = {
  articles: array(Article.t),
  bio: string,
  favfeedActiveClass: string,
  favoriteArticles: array(Article.t),
  image: string,
  isFavArticleDisplay: ReactDOMRe.style,
  isMyArticleDisplay: ReactDOMRe.style,
  myArticles: array(Article.t),
  myFeedActiveClass: string,
  showFavArticle: bool,
  showMyArticles: bool,
  username: string,
};

let initialState = {
  articles: [||],
  bio: "",
  favfeedActiveClass: "nav-link active",
  favoriteArticles: [||],
  image: "",
  isFavArticleDisplay: ReactDOMRe.Style.make(~display="none", ()),
  isMyArticleDisplay: ReactDOMRe.Style.make(~display="block", ()),
  myArticles: [||],
  myFeedActiveClass: "nav-link disabled",
  showFavArticle: false,
  showMyArticles: true,
  username: "",
};

type action =
  | CurrentUserFetched((string, string, string))
  | FavoriteArticle(array(Article.t))
  | MyArticles(array(Article.t))
  | NoData
  | PendingFavoriteArticles
  | PendingMyArticles;

let reduceMyArtcles = (reduceFunc, _status, payload) =>
  payload
  |> Js.Promise.then_(result => {
       let parsedArticles = Js.Json.parseExn(result);

       let articleList =
         Json.Decode.{
           articles:
             parsedArticles |> field("articles", array(Article.fromJson)),
           articlesCount: parsedArticles |> field("articlesCount", int),
         };
       reduceFunc(articleList.articles);

       articleList |> Js.Promise.resolve;
     });

let clickMyArticles = (event, {ReasonReact.state, reduce}) => {
  event->ReactEvent.Mouse.preventDefault;
  let reduceFunc = articles => reduce(_ => MyArticles(articles), ());

  JsonRequests.getMyArticles(
    reduceMyArtcles(reduceFunc),
    state.username,
    Effects.getTokenFromStorage(),
  )
  |> ignore;

  reduce(_ => PendingMyArticles, ());
};

let clickProfileSettings = (router, event, {ReasonReact.state: _state}) => {
  event->ReactEvent.Mouse.preventDefault;
  DirectorRe.setRoute(router, "/settings");
};

let clickMyFavorites = (event, {ReasonReact.state, reduce}) => {
  event->ReactEvent.Mouse.preventDefault;
  let reduceFunc = articles => reduce(_ => FavoriteArticle(articles), ());
  JsonRequests.getFavoritedArticles(
    reduceMyArtcles(reduceFunc),
    state.username,
    Effects.getTokenFromStorage(),
  )
  |> ignore;

  reduce(_ => PendingFavoriteArticles, ());
};

/* side effect */
let reduceByAuthArticles =
    ({ReasonReact.state, reduce}, _status, jsonPayload) =>
  jsonPayload
  |> Js.Promise.then_(payload => {
       let jsonArticles = Js.Json.parseExn(payload);
       let articleCount =
         Json.Decode.(jsonArticles |> field("articlesCount", int));

       switch (articleCount) {
       | count when count > 0 =>
         reduce(_ => MyArticles(extractArticleList(jsonArticles)), ())
       | _ => reduce(_ => NoData, ())
       };

       payload |> Js.Promise.resolve;
     });

/* These functions were copied from  */
let goToArticle =
    (router, articleCallback, article, event, {ReasonReact.state}) => {
  event->ReactEvent.Mouse.preventDefault;
  articleCallback(article);
  DirectorRe.setRoute(router, "/article");
};

let renderArticle =
    (handle, router, articleCallback, isFavorites, index, article) =>
  <div key={string_of_int(index)} className="article-preview">
    <div>
      <div className="article-meta">
        {
          if (isFavorites) {
            <a href="profile.html">
              <img
                src={Belt.Option.getWithDefault(article.author.image, "")}
              />
            </a>;
          } else {
            <a href="#" />;
          }
        }
        <div className="info">
          <a href="" className="author">
            {ReasonReact.string(article.author.username)}
          </a>
          <span className="date">
            {
              ReasonReact.string(
                Js.Date.fromString(article.createdAt) |> Js.Date.toDateString,
              )
            }
          </span>
        </div>
        <button className="btn btn-outline-primary btn-sm pull-xs-right">
          <i className="ion-heart" />
          {ReasonReact.string(string_of_int(article.favoritesCount))}
        </button>
      </div>
      <a
        href="#"
        onClick={handle(goToArticle(router, articleCallback, article))}
        className="preview-link">
        <h1> {ReasonReact.string(article.title)} </h1>
        <p> {ReasonReact.string(article.description)} </p>
        <span> {ReasonReact.string("Read more...")} </span>
      </a>
    </div>
  </div>;

let component = ReasonReact.reducerComponent("Profile");
let make = (~articleCallback, ~router, _children) => {
  ...component,
  initialState: () => initialState,
  reducer: (action, state) =>
    switch (action) {
    | MyArticles(articleList) =>
      ReasonReact.Update({
        ...state,
        isMyArticleDisplay: ReactDOMRe.Style.make(~display="block", ()),
        isFavArticleDisplay: ReactDOMRe.Style.make(~display="none", ()),
        myArticles: articleList,
        myFeedActiveClass: "nav-link active",
        favfeedActiveClass: "nav-link disabled",
      })
    | FavoriteArticle(articleList) =>
      ReasonReact.Update({
        ...state,
        isMyArticleDisplay: ReactDOMRe.Style.make(~display="none", ()),
        isFavArticleDisplay: ReactDOMRe.Style.make(~display="block", ()),
        favoriteArticles: articleList,
        myFeedActiveClass: "nav-link disabled",
        favfeedActiveClass: "nav-link active",
      })
    | CurrentUserFetched((username, bio, image)) =>
      ReasonReact.Update({...state, username, bio, image})
    | NoData => ReasonReact.NoUpdate
    | PendingFavoriteArticles => ReasonReact.NoUpdate
    | PendingMyArticles => ReasonReact.NoUpdate
    },
  didMount: self => {
    let (username, bio, image) = Effects.getUserFromStorage();

    let currentUsername = Belt.Option.getWithDefault(username, "");
    let currentBio = Belt.Option.getWithDefault(bio, "");
    let currentImage = Belt.Option.getWithDefault(image, "");
    let token = Effects.getTokenFromStorage();

    JsonRequests.getMyArticles(
      reduceByAuthArticles(self),
      currentUsername,
      token,
    )
    |> ignore;
    self.reduce(
      _ => CurrentUserFetched((currentUsername, currentBio, currentImage)),
      (),
    );
    ReasonReact.NoUpdate;
  },
  render: ({state, reduce}) => {
    let mapi = Belt.Array.mapWithIndex;
    <div className="profile-page">
      <div className="user-info">
        <div className="container">
          <div className="row">
            <div className="col-xs-12 col-md-10 offset-md-1">
              <img src={state.image} className="user-img" />
              <h4> {ReasonReact.string(state.username)} </h4>
              <p> {ReasonReact.string(state.bio)} </p>
              <button
                className="btn btn-sm btn-outline-secondary action-btn"
                onClick={self.handle(clickProfileSettings(router))}>
                <i className="ion-plus-round" />
                {ReasonReact.string("Edit Profile Settings")}
              </button>
            </div>
          </div>
        </div>
      </div>
      <div className="container">
        <div className="row">
          <div className="col-xs-12 col-md-10 offset-md-1">
            <div className="articles-toggle">
              <ul className="nav nav-pills outline-active">
                <li className="nav-item">
                  <a
                    className={state.myFeedActiveClass}
                    href="#"
                    onClick={self.handle(clickMyArticles)}>
                    {ReasonReact.string("My Articles")}
                  </a>
                </li>
                <li className="nav-item">
                  <a
                    className={state.favfeedActiveClass}
                    href="#"
                    onClick={self.handle(clickMyFavorites)}>
                    {ReasonReact.string("Favorited Articles")}
                  </a>
                </li>
              </ul>
            </div>
            <div style={state.isMyArticleDisplay}>
              {
                mapi(
                  renderArticle(self.handle, router, articleCallback, false),
                  state.myArticles,
                )
                ->ReasonReact.array
              }
            </div>
            <div style={state.isFavArticleDisplay}>
              {
                mapi(
                  renderArticle(self.handle, router, articleCallback, true),
                  state.favoriteArticles,
                )
                ->ReasonReact.array
              }
            </div>
          </div>
        </div>
      </div>
    </div>;
  },
};
