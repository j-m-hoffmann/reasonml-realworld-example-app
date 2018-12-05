open Models;

let defaultArticle = {
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

type state = Article.t;

type action =
  | SetCurrentArticle(Article.t);

let articleCallback = (reduce, currentArticle) =>
  reduce(_ => SetCurrentArticle(currentArticle), ());

let component = ReasonReact.reducerComponent("Body");

/* Just like any other variant data can be carried around with variants with the routes */
let make = (~route, ~router, _children) => {
  ...component,
  initialState: () => defaultArticle,
  reducer: (action, _state) =>
    switch (action) {
    | SetCurrentArticle(current) => ReasonReact.Update(current)
    },
  render: self => {
    let select_subpage = route =>
      switch (route) {
      | Routes.Home =>
        <Home articleCallback={articleCallback(reduce)} router />
      | Routes.Register => <Register router />
      | Routes.Login => <Login router />
      | Routes.Settings => <Settings router />
      | Routes.Article => <Article router state />
      | Routes.CreateArticle => <CreateArticle router />
      | Routes.EditArticle => <Article router state />
      | Routes.Profile =>
        <Profile articleCallback={articleCallback(reduce)} router />
      };
    <div> <Header router /> {select_subpage(route)} <Footer /> </div>;
  },
};
