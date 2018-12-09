open Routes;

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
        | Home =>
          <Home articleCallback=(a => send(SetCurrentArticle(a))) router />
        | Register => <Register router />
        | Login => <Login router />
        | Settings => <Settings router />
        | Article => <Article article=state />
        | CreateArticle => <CreateArticle router />
        | EditArticle => <Article article=state />
        | Profile =>
          <Profile
            articleCallback=(a => send(SetCurrentArticle(a)))
            router
          />
        }
      }
      <Footer />
    </div>,
};
