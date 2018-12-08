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

type action =
  | CurrentUserFetched(string, string, string)
  | FavoriteArticles(array(Article.t))
  | GoToSettings
  | MyArticles(array(Article.t))
  | NoData;
/*| PendingFavoriteArticles;*/
/*| PendingMyArticles;*/

let reduceMyArticles = (reduceFunc, _status, payload) =>
  payload
  |> Js.Promise.then_(result => {
       let articleList = Js.Json.parseExn(result)->ArticleList.fromJson;
       reduceFunc(articleList.articles);
       articleList |> Js.Promise.resolve;
     });

let clickMyArticles = (event, {ReasonReact.state, send}) => {
  event->ReactEvent.Mouse.preventDefault;

  JsonRequests.getMyArticles(
    reduceMyArticles(articles => send(MyArticles(articles))),
    state.username,
    LocalStorage.getToken(),
  )
  |> ignore;
  /*send(PendingMyArticles);*/
};

let clickMyFavorites = (event, {ReasonReact.state, send}) => {
  event->ReactEvent.Mouse.preventDefault;

  JsonRequests.getFavoritedArticles(
    reduceMyArticles(articles => send(FavoriteArticles(articles))),
    state.username,
    LocalStorage.getToken(),
  )
  |> ignore;
  /*send(_ => PendingFavoriteArticles);*/
};

/* side effect */
let reduceByAuthArticles = (self, _status, jsonPayload) =>
  jsonPayload
  |> Js.Promise.then_(result => {
       let articleList = Js.Json.parseExn(result)->ArticleList.fromJson;

       switch (articleList.articlesCount) {
       | count when count > 0 =>
         self.ReasonReact.send(MyArticles(articleList.articles))
       | _ => self.ReasonReact.send(NoData)
       };

       result |> Js.Promise.resolve;
     });

/* These functions were copied from  */
let goToArticle = (router, articleCallback, article, event, _self) => {
  event->ReactEvent.Mouse.preventDefault;
  articleCallback(article);
  DirectorRe.setRoute(router, "/article");
};

let renderArticle =
    (handle, router, articleCallback, isFavorites, index, article: Article.t) =>
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
            Js.Date.(fromString(article.createdAt)->toDateString)
            ->ReasonReact.string
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
  initialState: () => {
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
  },
  reducer: (action, state) =>
    switch (action) {
    | MyArticles(myArticles) =>
      ReasonReact.Update({
        ...state,
        isMyArticleDisplay: ReactDOMRe.Style.make(~display="block", ()),
        isFavArticleDisplay: ReactDOMRe.Style.make(~display="none", ()),
        myArticles,
        myFeedActiveClass: "nav-link active",
        favfeedActiveClass: "nav-link disabled",
      })
    | FavoriteArticles(favoriteArticles) =>
      ReasonReact.Update({
        ...state,
        isMyArticleDisplay: ReactDOMRe.Style.make(~display="none", ()),
        isFavArticleDisplay: ReactDOMRe.Style.make(~display="block", ()),
        favoriteArticles,
        myFeedActiveClass: "nav-link disabled",
        favfeedActiveClass: "nav-link active",
      })
    | CurrentUserFetched(bio, image, username) =>
      ReasonReact.Update({...state, bio, image, username})
    | GoToSettings =>
      ReasonReact.SideEffects(
        (_ => DirectorRe.setRoute(router, "/settings")),
      )
    | NoData => ReasonReact.NoUpdate
    /*| PendingFavoriteArticles => ReasonReact.NoUpdate*/
    /*| PendingMyArticles => ReasonReact.NoUpdate*/
    },
  didMount: self => {
    let (bio, image, username) = LocalStorage.getUser();
    let bio' = Belt.Option.getWithDefault(bio, "");
    let image' = Belt.Option.getWithDefault(image, "");
    let username' = Belt.Option.getWithDefault(username, "");

    JsonRequests.getMyArticles(
      reduceByAuthArticles(self),
      username',
      LocalStorage.getToken(),
    )
    |> ignore;
    self.send(CurrentUserFetched(bio', image', username'));
  },
  render: ({state, send, handle}) => {
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
                onClick={
                  event => {
                    event->ReactEvent.Mouse.preventDefault;
                    send(GoToSettings);
                  }
                }>
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
                    onClick={handle(clickMyArticles)}>
                    {ReasonReact.string("My Articles")}
                  </a>
                </li>
                <li className="nav-item">
                  <a
                    className={state.favfeedActiveClass}
                    href="#"
                    onClick={handle(clickMyFavorites)}>
                    {ReasonReact.string("Favorited Articles")}
                  </a>
                </li>
              </ul>
            </div>
            <div style={state.isMyArticleDisplay}>
              {
                mapi(
                  state.myArticles,
                  renderArticle(handle, router, articleCallback, false),
                )
                ->ReasonReact.array
              }
            </div>
            <div style={state.isFavArticleDisplay}>
              {
                mapi(
                  state.favoriteArticles,
                  renderArticle(handle, router, articleCallback, true),
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
