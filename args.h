#ifndef ARGS_ARGS_H_INCLUDED
#define ARGS_ARGS_H_INCLUDED

// Command-line argument parsing library
//
// Pavel Pilař, Michael Pokorný
//
// The API is deliberately designed to be very simple for the user.
// The user only needs to register flags via AddBool/Int/String/Enum and call
// Parse in main().
//
// Use case (see main.cpp):
//     // Create variables to hold values of parsed flags.
//     // Consider wrapping those in 'struct { ... } flags;' to cleanly
//     // separate them from usual variables.
//     args::String hostname;
//     args::Enum protocol;
//     args::Int timeout_ms;
//     args::Bool verbose;
//
//     int main(int argc, char** argv) {
//       // Bind our variables to new flags.
//       args::AddString(&hostname, "hostname", args::REQUIRED,
//                       "Hostname of the synchronization server.");
//       args::AddEnum(&protocol, {"protocol", 'p'}, args::REQUIRED,
//                     "Synchronization protocol to use.",
//                     {"http", "ftp", "https"});
//       args::AddInt(&timeout_ms, 't', args::OPTIONAL,
//                    "Connection timeout in ms. Default value is 1000.",
//                    0, 60000);
//       args::AddBool(&verbose, {"verbose", 'v'}, args::OPTIONAL,
//                  "Enable verbose logging?");
//
//       // Parse command line flags. Die on error.
//       args::Parse(&argc, &argv);
//
//       // argc, argv now point to the original zeroth argument
//       // (usually the program name) and nonflags arguments.
//       std::vector<char*> synchronized_paths(argc);
//       copy(&argv[1], &argv[argc], synchronized_paths.begin());  // skip [0]
//       SetSynchronizedFiles(synchronized_paths);
//
//       ConnectWithTimeout(hostname.get(), protocol.get(),
//                          timeout_ms.present() ? timeout_ms.get() : 1000);
//       // ...
//     }
//
// A flag is a variable that can have a value assigned by passing certain
// arguments on the command line. Every flag has at least one name. Names
// must be unique.
// Names may be short or long.
// Short names are one character long. The only allowed characters are ASCII
// alphanumerics. Long names must be at least one character long, and
// characters allowed in long names are ASCII alphanumerics, underscores (_)
// and dashes (-). Long names must be distinct from any short names.
// Flags values can be assigned as follows:
//   * --flag_name=value / -f=value
//   * --flag_name="a long value with spaces" / -f="a long value with spaces"
//   * --flag_name / -f  (Boolean flags only; sets the flag to true)
// Flags may be assigned at most one value.
// A space may be used instead of the equals sign.
// A flag that has an assigned value is called 'present'.
//
// The command line is passed to the library as a pointer to the argc and argv
// arguments of main().
// TODO: What happens if there's a space within an argc component?

#include <climits>
#include <exception>
#include <string>
#include <utility>
#include <vector>

// All functions and types of the library are within the args namespace.
namespace args {

// Parse gets a pointer to argc and argv as given to main(), parses out
// flags and modifies argc and argv to contain only nonflag arguments.
// If flag parsing fails, Parse prints out an error message describing
// the problem and exits with exit code 1.
// If no long flag named "--help" or short flag named "-h" is registered,
// passing either of these flags calls Usage and exits with exit code 0.
//
// This method doesn't throw exceptions or indicate failure via a return value
// to make the most common use case simpler (most programs don't need anything
// but the default behavior).
void Parse(int* argc, char*** argv) {
  (void) argc;
  (void) argv;
}

// Prints out help about all registered flags to standard output.
void ShowUsage() {
}

// Tries to parse a command line same as Parse().
// Unlike Parse, errors are signalled by throwing args::ParseError instead
// of exitting. If parsing wasn't successful, argc and argv are untouched.
// NOTE: TryParse doesn't respond to "--help" and "-h".
void TryParse(int* argc, const char*** argv) {
  (void) argc;
  (void) argv;
  return false;
}

// Exception thrown by TryParse on errors.
// Derived classes may give more semantic information about the error.
class ParseError : public std::exception {
 public:
  virtual ~ParseError();

  const char* what() override;

  // Returns a human-readable description of the parse error.
  const char* Description() const;

  // If this error occurred while parsing a flag, returns
  // a pointer to the flag. Returns NULL otherwise.
  Flag* FailedFlag() const;
};

// Returns the help string used by ShowUsage.
// Useful for handling TryParse errors.
std::string GetUsage() {
  return "";
}

// The args::internal namespace contains implementation details that need
// to be visible in the header due to C++ rules.
namespace internal {

// A universal set of names for a flag. A 'name' can be a one-char short name,
// or a char* long description.
// FlagNames can be constructed from a std::initializer_list
// of short or long names to represent a flag with multiple names.
// In this case, at least one name must be given. The first name will
// be used as the "primary" name for purposes of building help in Usage().
struct FlagName {
 public:
  FlagName(const char* long_name) {
    (void) long_name;
  }
  FlagName(char short_name) {
    (void) short_name;
  }
  FlagName(const std::initializer_list<FlagName>& names) {
    (void) names;
  }
};

// Thrown if the user attempts to access flags before they are parsed
// by Parse or TryParse.
class CommandLineNotParsed : public std::exception {};
// Thrown if the user tries to read the value of an optional flag
// that wasn't passed on the command line.
class FlagNotPassed : public std::exception {};
// Thrown if flag registration methods were called badly, for example
// if a name is repeated.
class FlagSpecificationError : public std::exception {};

// Subclasses of Flag<T> represent flags of various types.
template<typename T>
class Flag {
 public:
  // Returns the value given to this flag, if the flag was specified.
  // If the flag wasn't specified, throws FlagNotPassed.
  // Throws CommandLineNotParsed if Parse or TryParse wasn't called yet.
  virtual T get() = 0;
  // Returns whether this flag was given a value on the command-line.
  // Throws CommandLineNotParsed if Parse or TryParse wasn't called yet.
  virtual bool present() = 0;
};

class StringFlag : public Flag<std::string> {
 public:
  std::string get() override {
    return "";
  }
  bool present() override {
    return false;
  }
};

class EnumFlag : public StringFlag {};

class IntFlag : public Flag<int> {
 public:
  int get() override {
    return 0;
  }
  bool present() override {
    return false;
  }
};

class BoolFlag : public Flag<bool> {
 public:
  bool get() override {
    return false;
  }
  bool present() override {
    return false;
  }
};

}  // namespace internal

// In the context of the library, flags are represented as internal::Flag
// subclasses (internal::BoolFlag, internal::IntFlag, ...).
// These internal names are exposed as args::Bool, args::Int, ... to make usage
// simpler (e.g. "args::String my_flag" instead of "args::StringFlag my_flag").
typedef internal::StringFlag String;
typedef internal::EnumFlag Enum;
typedef internal::IntFlag Int;
typedef internal::BoolFlag Bool;

// Denotes whether a flag is required or optional.
enum FlagUse { REQUIRED, OPTIONAL };

// AddString, AddEnum, AddInt, AddBool create new flags.
// See top of this file for examples.
void AddString(internal::StringFlag* flag, const internal::FlagName& name,
               FlagUse use, const std::string& usage) {
  (void) flag;
  (void) name;
  (void) use;
  (void) usage;
}

void AddEnum(internal::EnumFlag* flag, const internal::FlagName& name,
             FlagUse use, const std::string& usage,
             std::initializer_list<const char*> allowed_values) {
  (void) flag;
  (void) name;
  (void) use;
  (void) usage;
  (void) allowed_values;
}

// The 'minimum' and 'maximum' parameters can be used to set the allowed range.
// The bounds are inclusive (minimum = 1, maximum = 3 allows {1, 2, 3}).
void AddInt(internal::IntFlag* flag, const internal::FlagName& name,
            FlagUse use, const std::string& usage,
            int minimum = INT_MIN, int maximum = INT_MAX) {
  (void) flag;
  (void) name;
  (void) use;
  (void) usage;
  (void) minimum;
  (void) maximum;
}

void AddBool(internal::BoolFlag* flag, const internal::FlagName& name,
             FlagUse use, const std::string& usage) {
  (void) flag;
  (void) name;
  (void) use;
  (void) usage;
}

// -- NOTES --
//
// Our initial idea was to also include 'operator T()' equivalent to 'get()'.
// We becided against that because even with 'operator T()', Flag<T> instances
// wouldn't be fully usable as T instances.
// For example, this wouldn't work due to C++ type inference rules:
//     cout << my_string_flag << endl;
// We would need to write:
//     cout << my_string_flag.get() << endl;
// On the other hand, this would work:
//     string my_flag_value = my_string_flag;
//
// Removing 'operator T()' makes the API more consistent (always 'flag.get()').
// Also, 'get()' throws exceptions when accessing nonpresent flags,
// so 'operator T()' would throw exceptions, too.
// 'string value = output_file.get();' makes this possibility more apparent
// than 'string value = output_file;', which looks like plain assignment.
//
// Short boolean flags can be "squashed" -- the command line "ls -laHrt" is
// equivalent to "ls -l -a -H -r -t".
//
// The '(void) something;' construct used in empty functions is used to
// squash "unused argument" warnings.
//
// Strictly speaking, the assignment asked to implement "flags that aren't
// allowed to have a value". This makes sense with booleans, but since it would
// be impossible to pass a non-bool flag without a value, this makes no
// sense for non-bools. Thus we decided to skip this part of the assignment
// and to only allow required and optional flags.

}  // namespace args

#endif
