type t = {
  id: int,
  createdAt: string,
  updatedAt: string,
  body: string,
  author: Author.t,
};

let fromJson = json =>
  Json.Decode.{
    id: json |> field("id", int),
    createdAt: json |> field("createdAt", string),
    updatedAt: json |> field("updatedAt", string),
    body: json |> field("body", string),
    author: json |> field("author", Author.fromJson),
  };

let component = ReasonReact.statelessComponent("Comment");

let make = (~comment: t, ~index, ~deleteComment, _children) => {
  ...component,
  render: _self =>
    <div className="card" key={string_of_int(index)}>
      <div className="card-block">
        <p className="card-text"> {ReasonReact.string(comment.body)} </p>
      </div>
      <div className="card-footer">
        <a href="" className="comment-author">
          <img
            src={Belt.Option.getWithDefault(comment.author.image, "")}
            className="comment-author-img"
          />
        </a>
        {ReasonReact.string(" ")}
        <a href="" className="comment-author">
          {ReasonReact.string(comment.author.username)}
        </a>
        <span className="date-posted">
          {
            ReasonReact.string(
              Js.Date.fromString(comment.createdAt) |> Js.Date.toDateString,
            )
          }
        </span>
        <span className="mod-options">
          <i className="ion-trash-a" onClick=deleteComment />
        </span>
      </div>
    </div>,
};
