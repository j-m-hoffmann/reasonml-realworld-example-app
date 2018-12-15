open Routes;

type state = {
  currentArticle: Article.t,
  token: option(string),
};

type action =
  | SetCurrentArticle(Article.t)
  | SetToken(option(string));

let component = ReasonReact.reducerComponent("Body");

/* Just like any other variant data can be carried around with variants with the routes */
let make = (~route, ~router, _children) => {
  ...component,
  initialState: () => {currentArticle: Article.empty, token: None},
  reducer: (action, state) =>
    switch (action) {
    | SetCurrentArticle(currentArticle) =>
      ReasonReact.Update({...state, currentArticle})
    | SetToken(token) => ReasonReact.Update({...state, token})
    },
  didMount: self => self.send(SetToken(LocalStorage.token())),
  render: ({send, state}) =>
    <div>
      <Header router token={state.token} />
      {switch (route) {
       | Article => <Article article={state.currentArticle} />
       | CreateArticle => <CreateArticle router />
       | EditArticle => <Article article={state.currentArticle} />
       | Home =>
         <Home articleCallback={a => send(SetCurrentArticle(a))} router />
       | Login => <Login logIn={t => send(SetToken(t))} router />
       | Profile =>
         <Profile articleCallback={a => send(SetCurrentArticle(a))} router />
       | Register => <Register logIn={t => send(SetToken(t))} router />
       | Settings => <Settings router />
       }}
      <Footer />
    </div>,
};
