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

let empty = Json.parseOrRaise({j|{}|j});

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
        test("should return login errors", () =>
          switch (fromJson(loginErrors)) {
          | Errors(_) => pass
          | User(_) => fail("Failed to find login errors")
          }
        );
        test("should return sign up errors", () =>
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
        test("should have 'invalid email' error", () =>
          switch (fromJson(signUpErrors)) {
          | Errors(errors) =>
            expect(
              Belt.Array.someU(errors, (. e) => e == "Email is invalid"),
            )
            |> toBe(true)
          | User(_) => fail("Failed to return errors")
          }
        );
        test("should have 'password is too short' error", () =>
          switch (fromJson(signUpErrors)) {
          | Errors(errors) =>
            expect(
              Belt.Array.someU(errors, (. e) =>
                e == "Password is too short (minimum is 8 characters)"
              ),
            )
            |> toBe(true)
          | User(_) => fail("Failed to return errors")
          }
        );
        test("should have the correct username", () =>
          switch (fromJson(authSuccess)) {
          | User(u) => expect(u.username) |> toBe("bryant")
          | Errors(_) => fail("Failed to return errors")
          }
        );
        test("should have 'no data' error", () =>
          switch (fromJson(empty)) {
          | Errors(errors) =>
            expect(
              Belt.Array.someU(errors, (. e) => e == "No Data received"),
            )
            |> toBe(true)
          | User(_) => fail("Returned user even though input was empty")
          }
        );
      }
    ),
  );
