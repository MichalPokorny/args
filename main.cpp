#include "args.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;

struct {
  args::String output_file;
  args::Enum mode;
  args::Int print_lines;
  args::Bool verbose;
} Options;

int main(int argc, char** argv) {
  (void) argc;
  (void) argv;
  args::AddString(&Options.output_file, {"output", 'o', "output_file"}, true,
		          "Where the output goes.");

  args::AddEnum(&Options.mode, {"mode", 'm'}, args::OPTIONAL,
                "An awesome flag.", {"x", "y", "z"});

  args::AddInt(&Options.print_lines, "print", args::OPTIONAL,
               "Some number flag.", 10, 35);

  args::AddBool(&Options.verbose, 'v', args::OPTIONAL, "Is verbose mode on?");

  args::Parse(&argc, &argv);

  if (Options.output_file.present()) {
    cout << Options.output_file.get() << endl;
  }

  cout << "it compiles" << endl;
  return 0;
}
