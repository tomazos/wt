#include <boost/filesystem.hpp>
#include <iostream>
#include <map>
#include <vector>

#include "primitives.h"
#include "must.h"
#include "process.h"

using namespace boost::filesystem;
using boost::filesystem::path;

class Whee {
 public:
  void Main(std::vector<string> args) {
    if (args.empty()) {
      args.push_back("help");
    }
    string subcommand_name = args[0];
    std::vector<string> rest_of_args(args.begin() + 1, args.end());

    using SubcommandFunction =
        std::function<void(const std::vector<string>& rest_of_args)>;
    static const std::map<string, SubcommandFunction> subcommand_function_map =
        {{"help", [&](const std::vector<string>& x) { return Help(x); }},
         {"init", [&](const std::vector<string>& x) { return Init(x); }},
         {"tidy", [&](const std::vector<string>& x) { return Tidy(x); }},
         {"build", [&](const std::vector<string>& x) { return Build(x); }}};

    const SubcommandFunction* subcommand_function =
        FindOrNull(subcommand_function_map, subcommand_name);
    if (subcommand_function)
      (*subcommand_function)(rest_of_args);
    else
      FAIL("No such subcommand: whee ", subcommand_name);
  }

 private:
  optional<path> source_root_, whee_dir_, git_dir_;

  const path& GetSourceRoot() {
    if (!source_root_) {
      source_root_ = FindSourceRoot();
    }
    MUST(source_root_, "Not inside a whee source tree.");
    return *source_root_;
  }
  const path& GetWheeDir() {
    if (!whee_dir_) {
      whee_dir_ = GetSourceRoot() / ".whee";
    }
    return whee_dir_.value();
  }
  const optional<path>& GetGitDir() {
    if (!git_dir_) {
      const path git_dir = GetSourceRoot() / ".git";
      if (exists(git_dir)) {
        git_dir_ = git_dir;
      }
    }
    return git_dir_;
  }

  void Help(const std::vector<string>& args) {
    static constexpr char helptext[] = R"(
  Whee is a C++ build system.

  Usage:

      $ whee <subcommand> [...]

  Subcommands:

   - whee help: this message
   - whee init <dir>: Sets up dir as a source tree root.
   - whee tidy: tidies up the source tree.
  )";

    static constexpr char helptext_init[] = R"(
  Usage:

      $ whee init <dir>

  Sets up dir as a source tree root.  This creates and populates a .whee folder.
  )";

    if (args.empty()) {
      std::cout << helptext + 1 << std::endl;
    } else {
      string subcommand = args[0];
      if (subcommand == "init") {
        std::cout << helptext_init + 1 << std::endl;
      } else {
        FAIL("Unknown subcommand: whee ", subcommand);
      }
    }
  }

  static bool ContainsWheeDirectory(const path& candidate_source_root) {
    const path whee_dir = candidate_source_root / ".whee";
    return is_directory(whee_dir);
  }

  static optional<path> FindSourceRoot(
      const path& start_path = current_path()) {
    MUST(exists(start_path), start_path, "is not a directory.");
    for (path p = start_path; exists(p); p = p.parent_path())
      if (ContainsWheeDirectory(p)) return p;

    return nullopt;
  }

  void Init(const std::vector<string>& args) {
    MUST_EQ(1u, args.size(), "Usage: whee init <dir>");
    const path source_root(args[0]);
    MUST(!FindSourceRoot(source_root));
    MUST(is_directory(source_root), source_root, " is not a directory.");
    const path whee_dir = source_root / ".whee";
    MUST(!exists(whee_dir), whee_dir,
         " already exists.  Delete it first to re-initialize.");
    create_directory(whee_dir);
  }

  void TidyFile(const path& p) {
    ExecuteShellCommand("clang-format-3.6 -style=Google -i ", p);
  }

  void ForEachSourcePath(
      std::function<void(const path&)> source_path_function) {
    for (recursive_directory_iterator it(GetSourceRoot());
         it != recursive_directory_iterator(); ++it) {
      const path source_path = it->path();
      if (equivalent(source_path, GetWheeDir()) ||
          (GetGitDir() && equivalent(source_path, *GetGitDir())))
        it.no_push();
      else {
        source_path_function(source_path);
      }
    }
  }

  void Tidy(const std::vector<string>& args) {
    MUST(args.empty());

    ForEachSourcePath([&](const path& source_path) {
      const path extension = source_path.extension();
      if (extension == ".h" || extension == ".cc") {
        TidyFile(source_path);
      }
    });
  }

  void Build(const std::vector<string>& args) {
    const path source_root = GetSourceRoot();
    FAIL("Not yet implemented.");
  }
};

int main(int argc, char** argv) {
  std::vector<string> args(argv + 1, argv + argc);
  try {
    Whee whee;
    whee.Main(args);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}
