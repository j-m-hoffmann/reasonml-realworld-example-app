type state = {
  title: string,
  description: string,
  body: string,
  tags: string,
};

type action =
  | SubmitArticle
  | UpdateTitle(string)
  | UpdateDescription(string)
  | UpdateBody(string)
  | UpdateTags(string);

let toJson = newArticle =>
  Json.Encode.(
    object_([
      ("title", string(newArticle.title)),
      ("description", string(newArticle.description)),
      ("body", string(newArticle.body)),
      ("tagList", newArticle.tags |> Js.String.split(",") |> stringArray),
    ])
  );

/* TODO: Add validation for body and title to be required */

let component = ReasonReact.reducerComponent("CreateArticle");
let make = (~router, _children) => {
  ...component,
  initialState: () => {title: "", description: "", body: "", tags: ""},
  reducer: (action, state) =>
    switch (action) {
    | SubmitArticle =>
      ReasonReact.SideEffects(
        (
          _self => {
            Request.Article.submit(toJson(state), ~f=(_status, body) =>
              body
              |> Js.Promise.then_(result =>
                   Js.log(result) |> Js.Promise.resolve
                 )
            )
            |> ignore;
            DirectorRe.setRoute(router, "/home");
          }
        ),
      )
    | UpdateTitle(title) => ReasonReact.Update({...state, title})
    | UpdateDescription(description) =>
      ReasonReact.Update({...state, description})
    | UpdateBody(body) => ReasonReact.Update({...state, body})
    | UpdateTags(tags) => ReasonReact.Update({...state, tags})
    },
  render: ({state, send}) =>
    <div className="editor-page">
      <div className="container page">
        <div className="row">
          <div className="col-md-10 offset-md-1 col-xs-12">
            <form>
              <fieldset>
                <fieldset className="form-group">
                  <input
                    type_="text"
                    className="form-control form-control-lg"
                    placeholder="Article Title"
                    value={state.title}
                    onChange={
                      event =>
                        send(
                          UpdateTitle(ReactEvent.Form.target(event)##value),
                        )
                    }
                  />
                </fieldset>
                <fieldset className="form-group">
                  <input
                    type_="text"
                    className="form-control"
                    placeholder="What's this article about?"
                    value={state.description}
                    onChange={
                      event =>
                        send(
                          UpdateDescription(
                            ReactEvent.Form.target(event)##value,
                          ),
                        )
                    }
                  />
                </fieldset>
                <fieldset className="form-group">
                  <textarea
                    className="form-control"
                    rows=8
                    placeholder="Write your article (in markdown)"
                    value={state.body}
                    onChange={
                      event =>
                        send(
                          UpdateBody(ReactEvent.Form.target(event)##value),
                        )
                    }
                  />
                </fieldset>
                <fieldset className="form-group">
                  <input
                    type_="text"
                    className="form-control"
                    placeholder="Enter tags"
                    value={state.tags}
                    onChange={
                      event =>
                        send(
                          UpdateTags(ReactEvent.Form.target(event)##value),
                        )
                    }
                  />
                  <div className="tag-list" />
                </fieldset>
                <button
                  className="btn btn-lg pull-xs-right btn-primary"
                  type_="button"
                  onClick={
                    event => {
                      event->ReactEvent.Mouse.preventDefault;
                      send(SubmitArticle);
                    }
                  }>
                  {ReasonReact.string("Publish Article")}
                </button>
              </fieldset>
            </form>
          </div>
        </div>
      </div>
    </div>,
};
