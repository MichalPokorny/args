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
  args::AddString(&Options.output_file, "output", true,
		  "Where the output goes.");
  args::Alias(&Options.output_file, 'o');
  args::Alias(&Options.output_file, "output_file");
  args::Alias(&Options.output_file, {'f', "output_content_file"});
  Options.output_file.alias({'x', 'y', "z", "foobar"});

  args::AddEnum(&Options.mode, "mode", args::OPTIONAL,
                {"hello", "world", "foobar"}, "An awesome flag.");

  args::AddInt(&Options.print_lines, "print", args::OPTIONAL, 10, 50,
		  "Some number flag.");

  args::AddBool(&Options.verbose, 'v', args::OPTIONAL, "Is verbose mode on?");

  args::Parse(&argc, &argv);

  if (Options.output_file.present()) {
    cout << Options.output_file.get() << endl;
  }

  cout << "it compiles" << endl;
  return 0;
}
