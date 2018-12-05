type user = {
  id: int,
  email: string,
  createdAt: string,
  updatedAt: string,
  username: string,
  bio: option(string),
  image: option(string),
  token: string,
};

type registeredUser = {
  errors: option(Errors.t),
  user,
};

type articleDetails = {
  title: string,
  description: string,
  body: string,
  tags: array(string),
};

type newArticle = {article: articleDetails};

type article = {
  slug: string,
  title: string,
  description: string,
  body: string,
  tagList: array(string),
  createdAt: string,
  updatedAt: string,
  favorited: bool,
  favoritesCount: int,
  author: Author.t,
};

type articleList = {
  articles: array(Article.t),
  articlesCount: int,
};
