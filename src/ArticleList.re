type t = {
  articles: array(Article.t),
  articlesCount: int,
};

/*let fromJson = json =>*/
/*Json.Decode.(json |> field("articles", array(Article.fromJson)));*/

let fromJson = json =>
  Json.Decode.{
    articles: json |> field("articles", array(Article.fromJson)),
    articlesCount: json |> field("articlesCount", int),
  };
