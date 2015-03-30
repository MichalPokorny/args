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
//     args::String output_file;
//     args::Enum protocol;
//     args::Int timeout_ms;
//     args::Bool verbose;
//     // Bind our variables to new flags.
//     args::AddString(&output_file, "output", args::REQUIRED,
//                     "Path to the output file.");
//     args::AddEnum(&protocol, "protocol", )
// TODO: is it a "flag", or an "option"? unify nomenclature

#include <string>
#include <vector>
#include <utility>

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
//
// Intended usage:
//   int main(int argc, char** argv) {
//     flags::Parse(&argc, &argv);
//     // argc, argv now only contain non-option arguments.
//   }
void Parse(int* argc, char*** argv) {
}

// TODO: Should this display help and quit on --help?
// TODO: Is it OK to just say "flags OK" / "flags not OK"?
bool TryParse(int argc, const char** argv) {
}

// This method returns vector of std::string representing standart comand-line
// arguments(arguments that are not represented as args framework specifies or
// are listed after "--" symbol).
// 
// If there are no such arguments returns empty vector
const std::vector<std::string>& GetNonoption() {
  return *(new std::vector<std::string>);
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

  void alias(const FlagName& name) {
  }
  void alias(const std::initializer_list<FlagName>& names) {
  }

  // TODO: mozna explicitni set?
};

// TODO: BoolFlag, IntFlag

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

// To create a new flag, call an Add* method like this:
//    
// first parameter	- args::String variable
// name				- name of new flag
// bool				- is flag required(true) or optional(false)
// documentation	- string documentation of flag
//
// Returns new string flag
internal::StringFlag& AddString(internal::StringFlag*,
                                const internal::FlagName& name,
                                bool required,
                                const std::string& documentation) {
  return *(new internal::StringFlag);
}

internal::EnumFlag& AddEnum(internal::EnumFlag*,
                            const internal::FlagName& name, bool required, 
							const std::string& documentation,
                            const std::initializer_list<std::string>&
                                allowed_values) {
  return *(new internal::EnumFlag);
}

internal::EnumFlag& AddEnum(internal::EnumFlag*,
                            const internal::FlagName& name, bool required,
							const std::string& documentation,
                            std::initializer_list<const char*> allowed_values
		            ) {
  return *(new internal::EnumFlag);
}

internal::IntFlag& AddInt(internal::IntFlag*, const internal::FlagName& name,
							bool required, const std::string& documentation,
							int lowerBound = INT_MIN,
							int upperBound = INT_MAX) {
}

internal::BoolFlag& AddBool(internal::BoolFlag*,
							const internal::FlagName& name, bool required,
							const std::string& documentation) {
}

void Alias(internal::StringFlag*, const internal::FlagName& name) {
}

void Alias(internal::StringFlag*,
           const std::initializer_list<internal::FlagName>& names) {
}

}  // namespace args
