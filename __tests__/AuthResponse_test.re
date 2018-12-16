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
          switch (toResult(loginErrors)) {
          | Error(_) => pass
          | Ok(_) => fail("Failed to find login errors")
          }
        );
        test("should find and return sign up errors", () =>
          switch (toResult(signUpErrors)) {
          | Error(_) => pass
          | Ok(_) => fail("Failed to find sign up errors")
          }
        );
        test("should succeed", () =>
          switch (fromJson(authSuccess).errors) {
          | None => pass
          | Some(_) => fail("errors present in successful authentication")
          }
        );
        test("should have an invalid email", () =>
          switch (fromJson(signUpErrors).errors) {
          | Some(errors) =>
            switch (errors.email) {
            | Some(error) => expect(error[0]) |> toBe("is invalid")
            | None => fail("this has failed")
            }
          | None => fail("this has failed")
          }
        );
        test("should have an error where the password is too short", () =>
          switch (fromJson(signUpErrors).errors) {
          | Some(errors) =>
            switch (errors.password) {
            | Some(password) =>
              expect(password[0])
              |> toBe("is too short (minimum is 8 characters)")
            | None => fail("Failed to check password validation")
            }
          | None => fail("Failed to return any errors")
          }
        );
        test("should have the correct username", () =>
          expect(fromJson(authSuccess).user.username) |> toBe("bryant")
        );
      }
    ),
  );
