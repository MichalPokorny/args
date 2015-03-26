#include <string>
#include <vector>
#include <utility>

namespace args {

// Preskoceno: "zakazane hodnoty optionu" pro string flagy, int flagy
// (jinde nedava smysl).

// TODO: "ls -laxHrtlm"

// int main(char** argv, int argc) {
//   flags::Parse(&argv, &argc);
//   // -> argv, argc jsou argumenty co nejsou options
// }
void Parse(int* argc, char*** argv) {
}

bool TryParse(int argc, const char** argv) {
}

void Usage() {
}

const std::vector<std::string>& GetNonoption() {
  return *(new std::vector<std::string>);
}

struct FlagName {
 public:
  FlagName(const std::string& name) {}
  FlagName(const char* name) {}
  FlagName(char short_name) {}
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

class StringFlag : public Flag<std::string> {
 public:
  std::string get() override {
    return "";
  }
  bool present() override {
    return false;
  }
};

class EnumFlag : public StringFlag {
 public:
  bool value_allowed(const std::string&) {
    return false;
  }
};

// Publicly exposed name: args::String, args::Enum.
typedef StringFlag String;
typedef EnumFlag Enum;

// struct {
//   flags::StringFlag my_string;
// } Options;
//
// flags::AddString(&Options.my_string, "output", REQUIRED, "this flag is useful");
// Options.my_string.alias('o', "output_thing")
// flags::Alias(&Options.my_string, 'o', "output_thing");
const bool REQUIRED = true;
const bool OPTIONAL = false;
StringFlag& AddString(StringFlag*, const FlagName& name, bool required, const std::string& documentation) {
  return *(new StringFlag);
}

// TODO: namapovat enum na C++ enum?

// TODO: template... ...
StringFlag& Alias(StringFlag*, const FlagName& name) {
}
StringFlag& Alias(StringFlag*, const std::initializer_list<FlagName>& names) {
}

EnumFlag& AddEnum(EnumFlag*, const FlagName& name, bool required, const std::initializer_list<std::string>& allowed_values,
		  const std::string& documentation) {
  return *(new EnumFlag);
}
EnumFlag& AddEnum(EnumFlag*, const FlagName& name, bool required, std::initializer_list<const char*> allowed_values,
		  const std::string& documentation) {
  return *(new EnumFlag);
}

// TODO: boolean, int

}
