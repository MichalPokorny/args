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
  args::AddBool(&verbose, {'v', "verbose"}, args::OPTIONAL,
                "Enable verbose logging");

  args::AddInt(&size, {'s', "size"}, args::OPTIONAL,
               "Size of something");

  // Parse command line flags. Die on error.
  args::Parse(&argc, &argv);
  
  if (verbose.present()){
    printf("verbose = true");
  } else {
    printf("verbose = false");
  }

  if (size.present()){
    printf("size = %i", size.get());
  } else {
    printf("size = 42");
  }
  
  // argc, argv now point to the original zeroth argument
  // (usually the program name) and nonflags arguments.
  std::vector<char*> non_flags(argc);
  copy(&argv[1], &argv[argc], non_flags.begin()); //skip 0

  //print non flags arguments
  cout << "args =";
  for (auto non_flag : non_flags){
    cout << non_flag << " ";
  }
  cout << endl;

  return 0;
}
