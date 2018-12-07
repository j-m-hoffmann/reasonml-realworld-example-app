let renderForRoute = (route, router) =>
  ReactDOMRe.renderToElementWithId(<Body route router />, "root");

let router =
  DirectorRe.makeRouter({
    "/": "root",
    "/home": "home",
    "/register": "register",
    "/login": "login",
    "/settings": "settings",
    "/article/create": "article_create",
    "/article/edit": "article_edit",
    "/article": "article",
    "/profile": "profile",
  });

let handlers =
  Routes.{
    "root": () => renderForRoute(Home, router),
    "home": () => renderForRoute(Home, router),
    "register": () => renderForRoute(Register, router),
    "login": () => renderForRoute(Login, router),
    "settings": () => renderForRoute(Settings, router),
    "article_create": () => renderForRoute(CreateArticle, router),
    "article_edit": () => renderForRoute(EditArticle, router),
    "article": () => renderForRoute(Article, router),
    "profile": () => renderForRoute(Profile, router),
  };

DirectorRe.configure(router, {"html5history": true, "resource": handlers});

DirectorRe.init(router, "/");
