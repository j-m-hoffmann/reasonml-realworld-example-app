
type articleDetails = {
  title: string,
  description: string,
  body: string,
  tags: array(string),
};

type newArticle = {article: articleDetails};

type articleList = {
  articles: array(Article.t),
  articlesCount: int,
};
