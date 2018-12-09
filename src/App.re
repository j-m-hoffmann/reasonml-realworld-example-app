open Routes;

let renderForRoute = (route, router) =>
  ReactDOMRe.renderToElementWithId(<Body route router />, "root");

let router =
  DirectorRe.makeRouter({
    "/": "root",
    "/article": "article",
    "/article/create": "article_create",
    "/article/edit": "article_edit",
    "/home": "home",
    "/login": "login",
    "/profile": "profile",
    "/register": "register",
    "/settings": "settings",
  });

let handlers = {
  "article": () => renderForRoute(Article, router),
  "article_create": () => renderForRoute(CreateArticle, router),
  "article_edit": () => renderForRoute(EditArticle, router),
  "home": () => renderForRoute(Home, router),
  "login": () => renderForRoute(Login, router),
  "profile": () => renderForRoute(Profile, router),
  "register": () => renderForRoute(Register, router),
  "root": () => renderForRoute(Home, router),
  "settings": () => renderForRoute(Settings, router),
};

DirectorRe.configure(router, {"html5history": true, "resource": handlers});

DirectorRe.init(router, "/");
