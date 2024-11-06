#define DEBUG

#include "automaton.h"
#include "dfa.h"
#include "epsnfa.h"
#include "nfa.h"
#include "out.h"

#include <filesystem>
#include <iostream>
#include <queue>

const std::string PATH_TO_CURRENT_AUTOMAT =
    "/home/sharovkv/Projects/University/"
    "Theory-of-formal-languages-and-translations/build/current_automaton";
const std::string FILE_EXTENSIONS[] = {".dfa", ".nfa", ".enfa"};

using Task = void(const std::string &);

std::filesystem::path FindAutomatonFile() {
  std::filesystem::path file;

  for (const auto &ext : FILE_EXTENSIONS) {
    if (std::filesystem::exists(PATH_TO_CURRENT_AUTOMAT + ext)) {
      file = PATH_TO_CURRENT_AUTOMAT + ext;
      break;
    }
  }

  return file;
}

std::unique_ptr<Automaton> Factory(const std::filesystem::path &path) {
  if (path.extension() == ".dfa") {
    return std::make_unique<DFA>(path);
  } else if (path.extension() == ".nfa") {
    return std::make_unique<NFA>(path);
  } else if (path.extension() == ".enfa") {
    return std::make_unique<ENFA>(path);
  } else {
    throw "Exception: Incorrect extension";
  }
}

void PrintHelp(const std::string &) {
  std::cout
      << "Usage:\n"
         "\tautomaton [-h | -help] [-A | --set-automaton "
         "<path/to/file>] [-P | --print-automaton] [-W | --word "
         "<word>]\n\n"
         "OPTIONS\n"
         "\t-h, --help\t\t\t\t\tshow list of command-line options\n"
         "\t-A, --set-automaton <path/to/file>\t\tset <path/to/file> "
         "to load automaton\n"
         "\t-P, --print-automaton\t\t\t\tshow table of automaton states and "
         "moves\n"
         "\t-W, --word <word>\t\t\t\tverificate the <word> with a "
         "current/given "
         "automaton; outputs response to stdout and details to stderr\n"
         "\t--convert-to-dfa\t\t\t\tconvert current automaton to "
         "deterministic\n"
         "\t--convert-to-nfa\t\t\t\tconvert current automaton to "
         "nondeterministic\n"
         "\t--convert-to-dfa\t\t\t\tconvert current automaton to "
         "nondeterministic with epsilon moves\n";
}

void SetAutomaton(const std::string &path_to_file) {
  std::filesystem::path file = path_to_file;

  if (!std::filesystem::exists(file)) {
    std::cerr << "automaton : Error: file on given path does not exists!\n";
    exit(1);
  }
  try {
    std::unique_ptr<Automaton> test = Factory(file);
  } catch (...) {
    std::cerr << "automaton : Error: json parsing error! Invalid file!\n";
    exit(1);
  }

  for (const auto &ext : FILE_EXTENSIONS) {
    if (std::filesystem::exists(PATH_TO_CURRENT_AUTOMAT + ext)) {
      std::filesystem::remove(PATH_TO_CURRENT_AUTOMAT + ext);
      break;
    }
  }

  if (!std::filesystem::copy_file(
          file, PATH_TO_CURRENT_AUTOMAT + file.extension().string(),
          std::filesystem::copy_options::overwrite_existing)) {
    std::cerr << "automaton : Error: setting automaton does not happened!\n";
    exit(1);
  }
}

void PrintAutomaton(const std::string &) {
  std::filesystem::path file = FindAutomatonFile();

  if (file.empty() || !std::filesystem::exists(file)) {
    std::cerr << "automaton : Error: automaton does not load!\nPlease use -A "
                 "<path/to/file> (or "
                 "--set-automaton <path/to/file>) command beforehand\n";
    exit(1);
  }

  std::unique_ptr<Automaton> automaton = Factory(file);
  Out(std::move(automaton->GetMovesTable()));
}

void ProcessWord(const std::string &word) {
  std::filesystem::path file = FindAutomatonFile();

  if (file.empty()) {
    std::cerr << "Erorr: automaton does not load!\nPlease use -A "
                 "<path/to/file> (or "
                 "--set-automaton <path/to/file>) command beforehand\n";
    exit(1);
  }

  std::unique_ptr<Automaton> automaton = Factory(file);

  std::cout << (automaton->InLanguage(word) ? word + " in the language\n"
                                            : word + " out of the language\n");
}

template <typename T> void ConvertTo(const std::string &) {
  std::filesystem::path file = FindAutomatonFile();

  if (file.empty()) {
    std::cerr << "Erorr: automaton does not load!\nPlease use -A "
                 "<path/to/file> (or "
                 "--set-automaton <path/to/file>) command beforehand\n";
    exit(1);
  }

  if (file.extension() == ".dfa") {
    T automaton = DFA(file);
#ifdef DEBUG
    Out(automaton.GetMovesTable());
#endif
  } else if (file.extension() == ".nfa") {
    T automaton = NFA(file);
#ifdef DEBUG
    Out(automaton.GetMovesTable());
#endif
  } else if (file.extension() == ".enfa") {
    T automaton = ENFA(file);
#ifdef DEBUG
    Out(automaton.GetMovesTable());
#endif
  }
}

struct TaskComparator {
  bool operator()(std::tuple<size_t, Task *, std::string> first,
                  std::tuple<size_t, Task *, std::string> second) {
    return std::get<size_t>(first) > std::get<size_t>(second);
  }
};

int main(int argc, char **argv) {
  using TaskWithParameter = std::tuple<size_t, Task *, std::string>;
  std::priority_queue<TaskWithParameter, std::vector<TaskWithParameter>,
                      TaskComparator>
      tasks;

  for (int i = 1; i < argc; ++i) {
    std::string argv_i = argv[i];

    if ((argv_i == "-A" || argv_i == "--set-automaton") && ++i != argc) {
      tasks.push(std::make_tuple(1U, SetAutomaton, argv[i]));
    } else if ((argv_i == "-W" || argv_i == "--word")) {
      tasks.push(std::make_tuple(3U, ProcessWord, ++i != argc ? argv[i] : ""));
    } else if (argv_i == "-P" || argv_i == "--print-automaton") {
      tasks.push(std::make_tuple(2U, PrintAutomaton, ""));
    } else if (argv_i == "--convert-to-dfa") {
      tasks.push(std::make_tuple(4U, ConvertTo<DFA>, ""));
    } else if (argv_i == "--convert-to-nfa") {
      tasks.push(std::make_tuple(4U, ConvertTo<NFA>, ""));
    } else if (argv_i == "--convert-to-enfa") {
      tasks.push(std::make_tuple(4U, ConvertTo<ENFA>, ""));
    } else {
      tasks.push(std::make_tuple(0U, PrintHelp, ""));
    }
  }

  while (!tasks.empty()) {
    auto &[priority, task, parameter] = tasks.top();
    task(parameter);
    if (!priority) {
      return 0;
    }
    tasks.pop();
  }
}
