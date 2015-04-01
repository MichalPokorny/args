#include "args.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;

// These are empty methods for use case to be able to build and run.
void ConnectWithTimeout(const std::string& hostname, const std::string& protocol, int timeout_ms){
  (void)hostname;
  (void)protocol;
  (void)timeout_ms;
}

void SetSynchronizedFiles(const std::vector<char*>& synchronized_paths){
  (void)synchronized_paths;
}

// Create variables to hold values of parsed flags.
args::String hostname;
args::Enum protocol;
args::Int timeout_ms;
args::Bool verbose;

int main(int argc, char** argv) { 
  // Bind our variables to new flags.
  args::AddString(&hostname, "hostname", args::REQUIRED,
                  "Hostname of the synchronization server.");
  args::AddEnum(&protocol, {"protocol", 'p'}, args::REQUIRED,
                "Synchronization protocol to use.",
                {"http", "ftp", "https"});
  args::AddInt(&timeout_ms, 't', args::OPTIONAL,
               "Connection timeout in ms. Default value is 1000.",
               0, 60000);
  args::AddBool(&verbose, {"verbose", 'v'}, args::OPTIONAL,
                "Enable verbose logging?");

  // Parse command line flags. Die on error.
  args::Parse(&argc, &argv);

  // argc, argv now point to the original zeroth argument
  // (usually the program name) and nonflags arguments.
  std::vector<char*> synchronized_paths(argc);
  copy(&argv[1], &argv[argc], synchronized_paths.begin());  // skip [0]
  SetSynchronizedFiles(synchronized_paths);

  ConnectWithTimeout(hostname.get(), protocol.get(),
                    timeout_ms.present() ? timeout_ms.get() : 1000);
  // ...

  return 0;
}