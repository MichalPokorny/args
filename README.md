# args

Command-line argument parser by @MichalPokorny and @bageta.
Assignment for NPRG043 Recommended Programming Practices on Charles University.

## Responses to reviewer critique
 * Our reviewers asked us to indent lines within `namespace`.
   We decided to keep `namespace` content unindented, because such is
   the decision of the Google C++ Style Guide.
 * We unified the name `documentation` with `GetUsage` to `usage` everywhere.
 * Originally our method for parsing flags without possibly dying had
   the signature `bool TryParse(int* argc, const char*** argv)`,
   which only gave one bit of information (pass/fail). We changed `TryParse`
   to `void` with throwing descriptive exceptions.
 * We enhanced the documentation of `AddBool`.
 * One of our reviewers suggested defaulting flag documentation to an empty
   string. We decided not to follow this advice -- our design forces users
   to document flags or to be annoyed by gross empty strings everywhere:
   `AddBool(&verbose, "verbose", args::REQUIRED, "")`.
   This is entirely intentional -- we want our users to document their flags.
 * The same reviewer also  suggests adding a "flag container"
   (something like `GetFlagValue<bool>("verbose")`). We decided not to:
   * A generic "flag container" would allow writing less DRY code with
     repetition of flag names:
       `AddFlag(&flag, "verbose", ...); GetFlagValue<bool>("verbose");`
     Such code can't have compile-time correctness checking and is brittle
     regarding flag renaming. It also forces the user to select a "primary
     flag name".
   * Our code has partial compile-time correctness guarantees. For example,
     `AddFlag(&x, ...)` forces the user to create a variable for the flag,
     which makes it harder to maintain unused flags (unused flags are bad).
   * Such a container can be easily implemented by an user of our library.
 * One reviewer flayed us for having many parameters in `Add*` functions
   and suggested using a builder pattern. We believe that a builder pattern
   would only add complexity, because all parameters (flag variable, flag
   name, requiredness, documentation) would still need to be stated
   explicitly. The builder pattern is useful when there are sane defaults,
   which there aren't -- for example, it's not obvious whether flags
   should be required by default or optional by default.
   The builder pattern would also make usage more complex by allowing
   permutations of the needed arguments. APIs should not allow more
   complexity than necessary.
 * We changed `bool required` to an enum.
