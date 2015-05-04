#include "args.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;

// Create variables to hold values of parsed flags.
// Consider wrapping those in 'struct { ... } flags;' to cleanly
// separate them from usual variables.
args::Int size;
args::Bool verbose;

int main(int argc, char** argv) {
  // Bind our variables to new flags.
  args::AddBool(&verbose, {'v', "verbose"}, args::FlagUse::OPTIONAL,
                "Enable verbose logging");
  args::AddInt(&size, {'s', "size"}, args::FlagUse::REQUIRED,
               "Size of something");

  // Parse command line flags. Die on error.
  args::Parse(&argc, &argv);
  
  // Check if "verbose" flag is present and true.
  if (verbose.present() && verbose.get()) {
    std::cout << "verbose = true" << std::endl;
  } else {
    std::cout << "verbose = false" << std::endl;
  }

  // Check if "size" flag is present. If so get its value.
  if (size.present()) {
    std::cout << "size = " <<  size.get() << std::endl;
  } else {
    std::cout << "size = 42" << std::endl;
  }
  
  // argc, argv now point to the original zeroth argument
  // (usually the program name) and nonflags arguments.
  std::vector<char*> non_flags(argc);
  if (argc > 1) {
    copy(&argv[1], &argv[argc], non_flags.begin()); // skip [0]

    // Print non flags arguments.
    std::cout << "args =";
    for (const auto& non_flag : non_flags) {
      std::cout << non_flag << " ";
    }
    std::cout << std::endl;
  }

  return 0;
}
