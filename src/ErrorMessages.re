/*let errorDisplayList = state =>*/
/*List.filter(*/
/*errorMessage => String.length(errorMessage) > 0,*/
/*state.errorList,*/
/*)*/
/*|> List.mapi((acc, errorMessage) =>*/
/*<ul className="error-messages" key={string_of_int(acc)}>*/
/*<li> {ReasonReact.string(errorMessage)} </li>*/
/*</ul>*/
/*);*/

let component = ReasonReact.statelessComponent("ErrorMessages");

let make = (~errors, _children) => {
  ...component,
  render: _self =>
    <ul className="error-messages">
      Belt.(
        List.keep(errors, errorMessage => String.length(errorMessage) > 0)
        ->List.toArray
        ->Array.mapWithIndex((i, errorMessage) =>
            <li key={string_of_int(i)}>
              {ReasonReact.string(errorMessage)}
            </li>
          )
        ->ReasonReact.array
      )
    </ul>,
};
