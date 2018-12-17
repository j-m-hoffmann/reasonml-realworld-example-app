open Jest;

open AuthResponse;

let authSuccess =
  Json.parseOrRaise(
    {j|{
      "user":{
        "id":12123,
        "email":"bryant@bryant.com",
        "createdAt":"2017-09-23T09:35:16.686Z",
        "updatedAt":"2017-09-23T09:35:16.691Z",
        "username":"bryant",
        "bio":null,
        "image":null,
        "token":"eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpZCI6MTIxMjMsInVzZXJuYW1lIjoiYnJ5YW50IiwiZXhwIjoxNTExMzQzMzE2fQ.WpeiSLOW2UUYrgeC0cgPkLY5v7aUC7yNKcIVMClgfCw"
      }
  }|j},
  );

let loginErrors =
  Json.parseOrRaise({j|{"errors":{"email or password":["is invalid"]}}|j});

let signUpErrors =
  Json.parseOrRaise(
    {j|{"errors":{"email":["is invalid"],"password":["is too short (minimum is 8 characters)"]}}|j},
  );

let () =
  describe(
    "New user request",
    ExpectJs.(
      () => {
        test("should find and return login errors", () =>
          switch (fromJson(loginErrors)) {
          | Errors(_) => pass
          | User(_) => fail("Failed to find login errors")
          }
        );
        test("should find and return sign up errors", () =>
          switch (fromJson(signUpErrors)) {
          | Errors(_) => pass
          | User(_) => fail("Failed to find sign up errors")
          }
        );
        test("should succeed", () =>
          switch (fromJson(authSuccess)) {
          | User(_) => pass
          | Errors(_) => fail("errors present in successful authentication")
          }
        );
        test("should have an invalid email", () =>
          switch (fromJson(signUpErrors)) {
          | Errors(e) => expect(e[0]) |> toBe("Email is invalid")
          | User(_) => fail("Failed to return errors")
          }
        );
        test("should have an error where the password is too short", () =>
          switch (fromJson(signUpErrors)) {
          | Errors(e) =>
            expect(e[1])
            |> toBe("Password is too short (minimum is 8 characters)")
          | User(_) => fail("Failed to return errors")
          }
        );
        test("should have the correct username", () =>
          switch (fromJson(authSuccess)) {
          | User(u) => expect(u.username) |> toBe("bryant")
          | Errors(_) => fail("Failed to return errors")
          }
        );
      }
    ),
  );
