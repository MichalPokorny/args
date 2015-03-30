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
//       // argc, argv now point to the original zeroth argument
//       // (usually the program name) and nonoption arguments.
//       vector<char*> synchronized_paths(argc);
//       copy(&argv[1], &argv[argc], synchronized_paths.begin());  // skip [0]
//       SetSynchronizedFiles(synchronized_paths);
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

// TODO: "ls -laxHrtlm"

// Prints out help about all registered options to standard output.
void Usage() {
}

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
}

// TODO: Should this display help and quit on --help?
// TODO: Is it OK to just say "flags OK" / "flags not OK"?
bool TryParse(int argc, const char** argv) {
}

// The args::internal namespace contains implementation details that need
// to be visible in the header due to C++ rules.
namespace internal {

struct FlagName {
 public:
  FlagName(const std::string& name) {}
  FlagName(const char* name) {}
  FlagName(char short_name) {}
  FlagName(const std::initializer_list<FlagName>& names) {}
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
const bool REQUIRED = true;
const bool OPTIONAL = false;

// AddString, AddEnum, AddInt, AddBool create new flags.
// See top of this file for examples.
internal::StringFlag& AddString(internal::StringFlag*,
                                const internal::FlagName& name,
                                bool required,
                                const std::string& documentation) {
  return *(new internal::StringFlag);
}

internal::EnumFlag& AddEnum(internal::EnumFlag*,
                            const internal::FlagName& name, bool required,
							const std::string& documentation,
                            std::initializer_list<const char*> allowed_values
		            ) {
  return *(new internal::EnumFlag);
}

// The 'minimum' and 'maximum' parameters can be used to set the allowed range.
// The bounds are inclusive (minimum = 1, maximum = 3 allows {1, 2, 3}).
internal::IntFlag& AddInt(internal::IntFlag*, const internal::FlagName& name,
					      bool required, const std::string& documentation,
						  int minimum = INT_MIN, int maximum = INT_MAX) {
}

internal::BoolFlag& AddBool(internal::BoolFlag*,
							const internal::FlagName& name, bool required,
							const std::string& documentation) {
}

// -- NOTES -- 
//
// Our initial idea was to also include 'operator T()' what would call .get().
// Eventually, we decided against that, mostly because even with this
// operator, flags would not be fully usable as their underlying types.
// For example, this wouldn't work due to C++ type inference rules:
//     cout << my_string_flag << endl;
// We would need to write:
//     cout << my_string_flag.get() << endl;
// On the other hand, this would work:
//     string my_flag_value = my_string_flag;
//
// We decided to remove the 'operator T()' to make the API more consistent
// (always 'flag.get()'). Also, because 'get()' throws exceptions when
// accessing nonpresent flags, 'operator T()' could throw exceptions as well.
// 'string value = my_string_flag;', however, looks innocent.
// 'string value = my_string_flag.get();' makes the possibility of failure
// more explicit.

}  // namespace args
