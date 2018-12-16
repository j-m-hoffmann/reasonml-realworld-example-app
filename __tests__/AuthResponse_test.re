open Jest;

open AuthResponse;

let signUpErrors =
  Json.parseOrRaise(
    {j|{"errors":{"email":["is invalid"],"password":["is too short (minimum is 8 characters)"]}}|j},
  );

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

let () =
  describe(
    "New user request",
    ExpectJs.(
      () => {
        test("should respond with an error", () => {
          let newUser = parseNewUser(signUpErrors);
          switch (newUser.errors) {
          | Some(_response) => pass
          | None => fail("Failed to find errors")
          };
        });
        test("should have an invalid email", () => {
          let newUser = parseNewUser(signUpErrors);
          switch (newUser.errors) {
          | Some(errorList) =>
            switch (errorList.email) {
            | Some(error) => expect(error[0]) |> toBe("is invalid")
            | None => fail("this has failed")
            }
          | None => fail("this has failed")
          };
        });
        test("should have an error where the password is too short", () => {
          let newUser = parseNewUser(signUpErrors);
          switch (newUser.errors) {
          | Some(errorList) =>
            switch (errorList.password) {
            | Some(password) =>
              expect(password[0])
              |> toBe("is too short (minimum is 8 characters)")
            | None => fail("Failed to check password validation")
            }
          | None => fail("Failed to return any errors")
          };
        });
        test("should have the correct username", () =>
          expect(parseNewUser(authSuccess).user.username) |> toBe("bryant")
        );
        test("should find and return errors", () =>
          switch (checkForErrors(loginErrors)) {
          | Some(_errors) => pass
          | None => fail("Failed to find errors")
          }
        );
      }
    ),
  );
