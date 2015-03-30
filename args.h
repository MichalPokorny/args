// Command-line argument parsing library
//
// Pavel Pilař, Michael Pokorný
//
// The API is deliberately designed to be very simple for the user.
// The user only needs to register options via AddBool/Int/String/Enum and call
// Parse in main().
//
// Strictly speaking, the assignment asked to implement "options that aren't
// allowed to have a value". This makes sense with booleans, but since it would
// be impossible to pass a non-bool option without a value, this makes no
// sense for non-bools. Thus we decided to skip this part of the assignment.
//
// Use case:
//     // Create variables to hold values of parsed flags.
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
//       args::Int(&timeout_ms, 't', args::OPTIONAL,
//                 "Connection timeout in ms. Default value is 1000.",
//                 0, 60000);
//       args::Bool(&verbose, {"verbose", 'v'}, args::OPTIONAL,
//                  "Enable verbose logging?");
//
//       // Parse command line flags. Die on error.
//       args::Parse(&argc, &argv);
//
//       ConnectWithTimeout(hostname.get(), protocol.get(),
//                          timeout_ms.present() ? timeout_ms.get() : 1000);
//       // ...
//     }
//
// TODO: is it a "flag", or an "option"? unify nomenclature

#include <climits>
#include <string>
#include <utility>
#include <vector>

// All functions and types of the library are within the args namespace.
namespace args {

// Parse gets a pointer to argc and argv as given to main(), parses out
// options and modifies argc and argv to contain only nonoption arguments.
// If option parsing fails, Parse prints out an error message describing
// the problem and exits with exit code 1.
// If no long option named "--help" or short option named "-h" is registered,
// passing either of these options calls Usage and exits with exit code 0.
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

// Tries to parse a command line same as Parse(). Returns true on success.
// Unlike Parse, errors are signalled by returning false instead of exitting.
// If parsing wasn't successful, argc and argv are untouched.
// NOTE: TryParse doesn't respond to "--help" and "-h".
bool TryParse(int* argc, const char*** argv) {
  (void) argc;
  (void) argv;
  return false;
}

// Returns the help string used by ShowUsage.
// Useful for handling TryParse errors.
std::string GetUsage() {
  return "";
}

// This method returns vector of std::string representing standard command-line
// arguments (arguments that are not represented as args framework specifies or
// are listed after "--" symbol).
const std::vector<std::string>& GetNonoption() {
  return *(new std::vector<std::string>);
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

template<typename T>
class Flag {
 public:
  virtual T get() = 0;
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

// Constants used to make code using args framework more clear.
// This may be possibly changed to an enum if there is need for more
// "flag types" than REQUIRED and OPTIONAL.
const bool REQUIRED = true;
const bool OPTIONAL = false;

// AddString, AddEnum, AddInt, AddBool create new flags.
// See top of this file for examples.
void AddString(internal::StringFlag* flag, const internal::FlagName& name,
               bool required, const std::string& documentation) {
  (void) flag;
  (void) name;
  (void) required;
  (void) documentation;
}

void AddEnum(internal::EnumFlag* flag, const internal::FlagName& name,
             bool required, const std::string& documentation,
             std::initializer_list<const char*> allowed_values) {
  (void) flag;
  (void) name;
  (void) required;
  (void) documentation;
  (void) allowed_values;
}

// The 'minimum' and 'maximum' parameters can be used to set the allowed range.
// The bounds are inclusive (minimum = 1, maximum = 3 allows {1, 2, 3}).
void AddInt(internal::IntFlag* flag, const internal::FlagName& name,
            bool required, const std::string& documentation,
            int minimum = INT_MIN, int maximum = INT_MAX) {
  (void) flag;
  (void) name;
  (void) required;
  (void) documentation;
  (void) minimum;
  (void) maximum;
}

void AddBool(internal::BoolFlag* flag, const internal::FlagName& name,
             bool required, const std::string& documentation) {
  (void) flag;
  (void) name;
  (void) required;
  (void) documentation;
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

}  // namespace args
