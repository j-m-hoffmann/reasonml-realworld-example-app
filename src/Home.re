type action =
  | ArticlesByPage(int)
  | ArticlesFetched(ArticleList.t)
  | FavoriteArticle(string, bool)
  | MyArticlesFetched(ArticleList.t)
  | ShowGlobalFeed
  | ShowMyFeed
  | ShowTagList(string)
  | TagArticlesFetched(ArticleList.t)
  | TagsFetched(array(string));

type state = {
  articlesCount: int,
  articles: array(Article.t),
  currentTagName: string,
  favoritedArticleSlug: string,
  globalFeedDisplay: ReactDOMRe.style,
  globalfeedActiveClass: string,
  myFeedActiveClass: string,
  myFeedDisplay: ReactDOMRe.style,
  showTagTab: bool,
  tagFeedActiveClass: string,
  tagFeedDisplay: ReactDOMRe.style,
  tags: array(string),
};

let applyAsync = (action_, _state, body) =>
  Js.Promise.(
    body
    |> then_(result =>
         Js.Json.parseExn(result)->ArticleList.fromJson->action_->resolve
       )
  );

let populateGlobalFeed = (self, page) =>
  /* Get the right page if there are more than 10 articles */
  Request.Article.all(
    ~page,
    ~f=
      applyAsync(articleList =>
        self.ReasonReact.send(ArticlesFetched(articleList))
      ),
  )
  |> ignore;

/*TODO into reducer */
let showMyFeed = (event, self) => {
  event->ReactEvent.Mouse.preventDefault;
  Request.Article.feed(
    ~f=
      applyAsync(articleList =>
        self.ReasonReact.send(MyArticlesFetched(articleList))
      ),
  )
  |> ignore;
  self.ReasonReact.send(ShowMyFeed);
};

/*TODO into reducer */
let showGlobalFeed = (event, self) => {
  event->ReactEvent.Mouse.preventDefault;
  populateGlobalFeed(self, 0);
  self.send(ShowGlobalFeed);
};

let goToArticle =
    (router, articleCallback, article, event, {ReasonReact.state: _state}) => {
  event->ReactEvent.Mouse.preventDefault;
  articleCallback(article);
  DirectorRe.setRoute(router, "/article");
};

let goToProfile = (router, event, {ReasonReact.state: _state}) => {
  event->ReactEvent.Mouse.preventDefault;
  DirectorRe.setRoute(router, "/profile");
};

let updateFavoritedCount = (articles, currentSlug) =>
  Belt.Array.map(articles, (article: Article.t) =>
    article.slug == currentSlug ?
      {
        ...article,
        favorited: !article.favorited,
        favoritesCount:
          article.favorited ?
            article.favoritesCount - 1 : article.favoritesCount + 1,
      } :
      article
  );

let mapWIU = Belt.Array.mapWithIndexU;

let renderTag = (self, index, tag) =>
  <a
    onClick={
      event =>
        self.ReasonReact.send(
          ShowTagList(ReactEvent.Mouse.target(event)##innerText),
        )
    }
    href="#"
    key={string_of_int(index)}
    className="tag-pill tag-default">
    {ReasonReact.string(tag)}
  </a>;

let renderPager = ({send, ReasonReact.handle}, articlesCount) => {
  let pageRanges = Belt.Array.makeBy(articlesCount / 10, i => i + 1);
  let reduceArticles = (currentPageNumber, event, _self) => {
    event->ReactEvent.Mouse.preventDefault;
    send(ArticlesByPage(currentPageNumber));
  };

  /* TODO Add the logic to highlight the current page */
  Belt.Array.map(pageRanges, currentPageNumber =>
    <li className="page-item ng-scope" key={string_of_int(currentPageNumber)}>
      <a
        className="page-link ng-binding"
        href=""
        onClick={handle(reduceArticles(currentPageNumber))}>
        {ReasonReact.string(string_of_int(currentPageNumber))}
      </a>
    </li>
  )
  ->ReasonReact.array;
};

let renderArticle =
    (
      {ReasonReact.state: _state, send},
      handle,
      router,
      articleCallback,
      index,
      article: Article.t,
    ) =>
  <div key={string_of_int(index)} className="article-preview">
    <div>
      <div className="article-meta">
        <a href="#" onClick={handle(goToProfile(router))}>
          <img src={Belt.Option.getWithDefault(article.author.image, "")} />
        </a>
        <div className="info">
          <a
            href="#"
            onClick={handle(goToProfile(router))}
            className="author">
            {ReasonReact.string(article.author.username)}
          </a>
          <span className="date">
            Js.Date.(fromString(article.createdAt)->toDateString)
            ->ReasonReact.string
          </span>
        </div>
        <button
          className="btn btn-outline-primary btn-sm pull-xs-right"
          onClick={
            _event => send(FavoriteArticle(article.slug, article.favorited))
          }>
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
        <ul className="tag-list">
          {
            mapWIU(article.tagList, (. i, tag) =>
              <li
                className="tag-default tag-pill tag-outline"
                key={string_of_int(i)}>
                {ReasonReact.string(tag)}
              </li>
            )
            ->ReasonReact.array
          }
        </ul>
      </a>
    </div>
  </div>;

let component = ReasonReact.reducerComponent("Home");

let make = (~articleCallback, ~router, _children) => {
  ...component,
  initialState: () => {
    articlesCount: 0,
    articles: [||],
    currentTagName: "",
    favoritedArticleSlug: "",
    globalFeedDisplay: ReactDOMRe.Style.make(~display="block", ()),
    globalfeedActiveClass: "nav-link active",
    myFeedActiveClass: "nav-link disabled",
    myFeedDisplay: ReactDOMRe.Style.make(~display="none", ()),
    showTagTab: false,
    tagFeedActiveClass: "nav-link disabled",
    tagFeedDisplay: ReactDOMRe.Style.make(~display="none", ()),
    tags: [||],
  },
  reducer: (action, state) =>
    switch (action) {
    | TagsFetched(tags) => ReasonReact.Update({...state, tags})
    | ShowMyFeed =>
      ReasonReact.Update({
        ...state,
        myFeedDisplay: ReactDOMRe.Style.make(~display="block", ()),
        globalFeedDisplay: ReactDOMRe.Style.make(~display="none", ()),
        tagFeedDisplay: ReactDOMRe.Style.make(~display="none", ()),
        myFeedActiveClass: "nav-link active",
        globalfeedActiveClass: "nav-link disabled",
        tagFeedActiveClass: "nav-link disabled",
      })
    | ShowGlobalFeed =>
      ReasonReact.Update({
        ...state,
        myFeedDisplay: ReactDOMRe.Style.make(~display="none", ()),
        globalFeedDisplay: ReactDOMRe.Style.make(~display="block", ()),
        tagFeedDisplay: ReactDOMRe.Style.make(~display="none", ()),
        myFeedActiveClass: "nav-link disabled",
        globalfeedActiveClass: "nav-link active",
        tagFeedActiveClass: "nav-link disabled",
      })
    | ArticlesFetched({articles, articlesCount}) =>
      ReasonReact.Update({
        ...state,
        articles,
        articlesCount,
        tagFeedDisplay: ReactDOMRe.Style.make(~display="none", ()),
      })
    | MyArticlesFetched({articles, articlesCount}) =>
      ReasonReact.Update({...state, articles, articlesCount})
    | TagArticlesFetched({articles}) =>
      ReasonReact.Update({
        ...state,
        articles,
        myFeedDisplay: ReactDOMRe.Style.make(~display="none", ()),
        globalFeedDisplay: ReactDOMRe.Style.make(~display="none", ()),
        tagFeedDisplay: ReactDOMRe.Style.make(~display="block", ()),
      })
    | ShowTagList(currentTagName) =>
      ReasonReact.UpdateWithSideEffects(
        {
          ...state,
          currentTagName,
          myFeedDisplay: ReactDOMRe.Style.make(~display="none", ()),
          globalFeedDisplay: ReactDOMRe.Style.make(~display="none", ()),
          tagFeedDisplay: ReactDOMRe.Style.make(~display="block", ()),
          myFeedActiveClass: "nav-link disabled",
          globalfeedActiveClass: "nav-link disabled",
          tagFeedActiveClass: "nav-link active",
        },
        (
          self =>
            Request.Article.byTag(
              currentTagName,
              ~f=
                applyAsync(articleList =>
                  self.send(TagArticlesFetched(articleList))
                ),
            )
            |> ignore
        ),
      )
    | FavoriteArticle(slug, isCurrentlyFav) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, articles: updateFavoritedCount(state.articles, slug)},
        (
          _self =>
            isCurrentlyFav ?
              Request.Article.unfavorite(slug) |> ignore :
              Request.Article.favorite(slug) |> ignore
        ),
      )
    | ArticlesByPage(page) =>
      ReasonReact.SideEffects(
        (
          self =>
            Request.Article.all(
              ~page,
              ~f=
                applyAsync(articleList =>
                  self.send(ArticlesFetched(articleList))
                ),
            )
            |> ignore
        ),
      )
    },
  didMount: self => {
    Request.Tags.all(~f=(_status, body) =>
      body
      |> Js.Promise.then_(result => {
           let json = Js.Json.parseExn(result);
           let tags = Json.Decode.(json |> field("tags", array(string)));
           self.send(TagsFetched(tags)) |> Js.Promise.resolve;
         })
    )
    |> ignore;
    populateGlobalFeed(self, 0);
  },
  render: ({state, send, handle} as self) => {
    let currentTagName = state.currentTagName;
    <div className="home-page">
      <div className="banner">
        <div className="container">
          <h1 className="logo-font"> {ReasonReact.string("conduit")} </h1>
          <p> {ReasonReact.string("A place to share your knowledge.")} </p>
        </div>
      </div>
      <div className="container page">
        <div className="row">
          <div className="col-md-9">
            <div className="feed-toggle">
              <ul className="nav nav-pills outline-active">
                <li className="nav-item">
                  <a
                    className={state.myFeedActiveClass}
                    href="#"
                    onClick={handle(showMyFeed)}>
                    {ReasonReact.string("Your Feed")}
                  </a>
                </li>
                <li className="nav-item">
                  <a
                    className={state.globalfeedActiveClass}
                    href="#"
                    onClick={handle(showGlobalFeed)}>
                    {ReasonReact.string("Global Feed")}
                  </a>
                </li>
                <li className="nav-item" style={state.tagFeedDisplay}>
                  <a className={state.tagFeedActiveClass} href="#">
                    {ReasonReact.string({j|#$currentTagName|j})}
                  </a>
                </li>
              </ul>
            </div>
            /*TODO this renders 3 times the same thing check again later*/
            <div style={state.myFeedDisplay}>
              {
                mapWIU(state.articles, (. i, article) =>
                  renderArticle(
                    self,
                    handle,
                    router,
                    articleCallback,
                    i,
                    article,
                  )
                )
                ->ReasonReact.array
              }
            </div>
            <div style={state.globalFeedDisplay}>
              {
                mapWIU(state.articles, (. i, article) =>
                  renderArticle(
                    self,
                    handle,
                    router,
                    articleCallback,
                    i,
                    article,
                  )
                )
                ->ReasonReact.array
              }
            </div>
            <div style={state.tagFeedDisplay}>
              {
                mapWIU(state.articles, (. i, article) =>
                  renderArticle(
                    self,
                    handle,
                    router,
                    articleCallback,
                    i,
                    article,
                  )
                )
                ->ReasonReact.array
              }
            </div>
            <div>
              <nav>
                <ul className="pagination">
                  {renderPager(self, state.articlesCount)}
                </ul>
              </nav>
            </div>
          </div>
          <div className="col-md-3">
            <div className="sidebar">
              <p> {ReasonReact.string("Popular Tags")} </p>
              <div className="tag-list">
                {
                  mapWIU(state.tags, (. i, tag) => renderTag(self, i, tag))
                  ->ReasonReact.array
                }
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>;
  },
};
