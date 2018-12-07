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

let populateTags = self => {
  let reduceTags = (_status, jsonPayload) =>
    jsonPayload
    |> Js.Promise.then_(result => {
         let parsedPopularTags = Js.Json.parseExn(result);
         let tags =
           Json.Decode.(parsedPopularTags |> field("tags", array(string)));
         self.ReasonReact.send(TagsFetched(tags));

         tags |> Js.Promise.resolve;
       })
    |> ignore;
  JsonRequests.getPoplarTags(reduceTags) |> ignore;
};

let reduceFeed = (reduceToAction, _state, jsonPayload) =>
  jsonPayload
  |> Js.Promise.then_(result => {
       let articleList = Js.Json.parseExn(result)->ArticleList.fromJson;
       reduceToAction(articleList);
       articleList |> Js.Promise.resolve;
     });

let populateGlobalFeed = (self, pageNumber) => {
  let reduceFunc = articleList =>
    self.ReasonReact.send(ArticlesFetched(articleList));

  /* Get the right page if there are more than 10 articles */
  JsonRequests.getGlobalArticles(
    reduceFeed(reduceFunc),
    LocalStorage.getToken(),
    10,
    pageNumber * 10,
  )
  |> ignore;
};

let populateMyFeed = self => {
  let reduceFunc = articleList =>
    self.ReasonReact.send(MyArticlesFetched(articleList));
  JsonRequests.getFeed(LocalStorage.getToken(), reduceFeed(reduceFunc))
  |> ignore;
};

let showMyFeed = (event, self) => {
  event->ReactEvent.Mouse.preventDefault;
  populateMyFeed(self);
  self.ReasonReact.send(ShowMyFeed);
};

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

let updateFavoritedCount = (articles, currentSlug) => {
  let updateCurrentArticle = (article: Article.t) =>
    /*let incDecFavCount =*/
    /*fun*/
    /*| true => article.favoritesCount + 1*/
    /*| false => article.favoritesCount - 1;*/
    article.slug == currentSlug ?
      {
        ...article,
        favorited: !article.favorited,
        favoritesCount:
          article.favorited ?
            article.favoritesCount - 1 : article.favoritesCount + 1,
      } :
      article;
  Belt.Array.map(articles, updateCurrentArticle);
};

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

let renderArticleTag = (index, tag) =>
  <li className="tag-default tag-pill tag-outline" key={string_of_int(index)}>
    {ReasonReact.string(tag)}
  </li>;

/*let rec range = (a, b) =>*/
/*if (a > b) {*/
/*[];*/
/*} else {*/
/*[a, ...range(a + 1, b)];*/
/*};*/

let renderPager = ({send, ReasonReact.handle}, articlesCount) => {
  let pageRanges = Belt.List.makeBy(articlesCount / 10, i => i + 1);
  /*let pageRanges = Belt.Array.makeBy(articlesCount / 10, i => i + 1);*/
  /*let pageRanges = range(1, articlesCount / 10);*/
  let reduceArticles = (currentPageNumber, event, _self) => {
    event->ReactEvent.Mouse.preventDefault;
    send(ArticlesByPage(currentPageNumber));
  };

  /* Add the logic to highlight the current page */
  List.map(
    currentPageNumber =>
      <li
        className="page-item ng-scope" key={string_of_int(currentPageNumber)}>
        <a
          className="page-link ng-binding"
          href=""
          onClick={handle(reduceArticles(currentPageNumber))}>
          {ReasonReact.string(string_of_int(currentPageNumber))}
        </a>
      </li>,
    pageRanges,
  )
  ->Array.of_list
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
            {
              ReasonReact.string(
                Js.Date.fromString(article.createdAt)->Js.Date.toDateString,
              )
            }
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
          {Array.mapi(renderArticleTag, article.tagList)->ReasonReact.array}
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
    | TagsFetched(tagList) => ReasonReact.Update({...state, tags: tagList})
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
          self => {
            let reduceFunc = articleList =>
              self.send(TagArticlesFetched(articleList));
            JsonRequests.getArticlesByTag(
              reduceFeed(reduceFunc),
              currentTagName,
              LocalStorage.getToken(),
            )
            |> ignore;
          }
        ),
      )
    | FavoriteArticle(slug, isCurrentlyFav) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, articles: updateFavoritedCount(state.articles, slug)},
        (
          _self =>
            !isCurrentlyFav ?
              JsonRequests.favoriteArticle(LocalStorage.getToken(), slug)
              |> ignore :
              JsonRequests.unfavoriteArticle(LocalStorage.getToken(), slug)
              |> ignore
        ),
      )
    | ArticlesByPage(currentPage) =>
      ReasonReact.SideEffects(
        (
          self => {
            let reduceFunc = articleList =>
              self.send(ArticlesFetched(articleList));
            JsonRequests.getGlobalArticles(
              reduceFeed(reduceFunc),
              LocalStorage.getToken(),
              10,
              currentPage * 10,
            )
            |> ignore;
          }
        ),
      )
    },
  didMount: self => {
    populateTags(self);
    populateGlobalFeed(self, 0);
  },
  render: ({state, handle}) => {
    let mapi = Belt.Array.mapWithIndex;
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
                mapi(state.articles, (i, article) =>
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
                mapi(state.articles, (i, article) =>
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
                mapi(state.articles, (i, article) =>
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
                  mapi(state.tags, (i, tag) => renderTag(self, i, tag))
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
