type state = Article.t;

type action =
  | SetCurrentArticle(Article.t);

let component = ReasonReact.reducerComponent("Body");

/* Just like any other variant data can be carried around with variants with the routes */
let make = (~route, ~router, _children) => {
  ...component,
  initialState: () => Article.empty,
  reducer: (action, _state) =>
    switch (action) {
    | SetCurrentArticle(current) => ReasonReact.Update(current)
    },
  render: ({send, state}) =>
    <div>
      <Header router />
      {
        switch (route) {
        | Routes.Home =>
          <Home articleCallback=(a => send(SetCurrentArticle(a))) router />
        | Routes.Register => <Register router />
        | Routes.Login => <Login router />
        | Routes.Settings => <Settings router />
        | Routes.Article => <Article article=state />
        | Routes.CreateArticle => <CreateArticle router />
        | Routes.EditArticle => <Article article=state />
        | Routes.Profile =>
          <Profile
            articleCallback=(a => send(SetCurrentArticle(a)))
            router
          />
        }
      }
      <Footer />
    </div>,
};
