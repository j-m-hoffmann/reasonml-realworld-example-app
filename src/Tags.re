type t = array(string);

let fromJson = json => Json.Decode.(json |> field("tags", array(string)));

let component = ReasonReact.statelessComponent("Tags");

let make = _children => {
  ...component,
  render: self => <div> {ReasonReact.string("Tags")} </div>,
};
