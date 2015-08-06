#include <boost/filesystem.hpp>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "primitives.h"
#include "must.h"
#include "process.h"
#include "token_tree.h"

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
    source_path_function(canonical(GetSourceRoot()));
    for (recursive_directory_iterator it(GetSourceRoot());
         it != recursive_directory_iterator(); ++it) {
      const path source_path = it->path();
      if (equivalent(source_path, GetWheeDir()) ||
          (GetGitDir() && equivalent(source_path, *GetGitDir())))
        it.no_push();
      else {
        source_path_function(canonical(source_path));
      }
    }
  }

  void Tidy(const std::vector<string>& args) {
    MUST(args.empty());

    ForEachSourcePath([&](const path& source_path) {
      const path extension = source_path.extension();
      if (extension == ".h" || extension == ".cc" || extension == ".c") {
        TidyFile(source_path);
      }
    });
  }

  struct RuleRef {
    string directory;
    string name;
    RuleRef() = default;
    RuleRef(const string& directory_in, const string& name_in)
        : directory(directory_in), name(name_in) {}

    bool operator==(const RuleRef& that) const {
      return directory == that.directory && name == that.name;
    }
    bool operator!=(const RuleRef& that) const { return !(*this == that); }
    bool operator<(const RuleRef& that) const {
      return directory < that.directory ||
             (directory == that.directory && name < that.name);
    }
    bool operator>(const RuleRef& that) const { return that < *this; }
    bool operator<=(const RuleRef& that) const { return !(*this > that); }
    bool operator>=(const RuleRef& that) const { return !(*this < that); }
  };

  struct Rule {
    string name;
    string full_name;
    enum RuleKind { LIBRARY, PROGRAM, TEST };
    RuleKind kind;
    std::vector<string> headers;
    std::vector<string> sources;
    std::vector<RuleRef> dependencies;
  };

  struct SourceDirectory {
    std::map<string, Rule> rules;
    std::set<string> files;
  };

  using SourceTree = std::map<string, SourceDirectory>;

  SourceTree GetSourceTree() {
    SourceTree source_tree;

    const path source_root = GetSourceRoot();
    const string source_root_string = source_root.string() + "/";
    const size_t source_root_strlen = source_root_string.size();
    ForEachSourcePath([&](const path& source_path) {
      if (!is_regular_file(source_path)) return;
      const path directory_path = canonical(source_path.parent_path());
      string directory = directory_path.string();
      if (directory + "/" == source_root_string) {
        directory = "";
      } else {
        MUST_EQ(source_root_string, directory.substr(0, source_root_strlen),
                directory);
        directory = directory.substr(source_root_strlen);
      }
      if (source_path.filename() != "RULES") {
        const string filename = source_path.filename().string();
        MUST(Insert(source_tree[directory].files, filename),
             "duplicate filename ", filename);
      } else {
        auto rules_sequence = token_tree::ParseSequenceFile(source_path);
        for (const auto& rule_element : rules_sequence->elements) {
          const auto* rule_sequence =
              dynamic_cast<token_tree::Sequence*>(rule_element.get());
          MUST(rule_sequence, "Unexpected element: ", rule_element->ToString());
          Rule rule;
          if (rule_sequence->key == "library") {
            rule.kind = Rule::LIBRARY;
          } else if (rule_sequence->key == "program") {
            rule.kind = Rule::PROGRAM;
          } else if (rule_sequence->key == "test") {
            rule.kind = Rule::TEST;
          } else {
            FAIL("Unknown rule: ", rule_sequence->key);
          }
          for (const auto& section_element : rule_sequence->elements) {
            if (const auto* section_keyval =
                    dynamic_cast<token_tree::KeyVal*>(section_element.get())) {
              if (section_keyval->key == "name") {
                MUST(rule.name.empty());
                rule.name = section_keyval->value;
              } else {
                FAIL("unknown keyval kind: ", section_keyval->key);
              }
            } else if (const auto* section_sequence =
                           dynamic_cast<token_tree::Sequence*>(
                               section_element.get())) {
              std::vector<string> pathlist;
              for (const auto& element : section_sequence->elements) {
                const auto* leaf =
                    dynamic_cast<token_tree::Leaf*>(element.get());
                pathlist.push_back(leaf->token);
              }

              if (section_sequence->key == "headers") {
                MUST(rule.headers.empty());
                rule.headers = std::move(pathlist);
              } else if (section_sequence->key == "sources") {
                MUST(rule.sources.empty());
                rule.sources = std::move(pathlist);
              } else if (section_sequence->key == "dependencies") {
                MUST(rule.dependencies.empty());
                for (const string& dependency : pathlist) {
                  MUST(!dependency.empty());
                  RuleRef rule_ref;
                  if (dependency[0] == '/') {
                    size_t pos = dependency.find_last_of("/");
                    if (pos == 0) {
                      rule_ref.directory = "";
                    } else {
                      rule_ref.directory = dependency.substr(1, pos - 1);
                    }
                    rule_ref.name = dependency.substr(pos + 1);
                  } else {
                    rule_ref.directory = directory;
                    rule_ref.name = dependency;
                  }
                  rule.dependencies.push_back(rule_ref);
                }
              } else {
                FAIL("unknown section sequence: ", section_sequence->key);
              }
            } else {
              FAIL("unexpected section in ", rule.name);
            }
          }
          Insert(source_tree[directory].rules, rule.name, rule);
        }
      }
    });
    return source_tree;
  }

  struct Platform {
    string name;
    string tool_prefix;
  };

  string CompileCommand(const Platform& platform, const path& source,
                        const path& object, const path& primitives_header,
                        const std::vector<path> include_paths) {
    std::vector<string> include_paths_strings;
    for (const path& include_path : include_paths)
      include_paths_strings.push_back("-I" + include_path.string());

    return EncodeAsString(platform.tool_prefix,
                          "g++ -c -g -std=gnu++14 -Wall -Werror -O3 ",
                          boost::algorithm::join(include_paths_strings, " "),
                          " -include ", primitives_header.string(), " -o ",
                          object.string(), " ", source.string());
  }

  string LibraryCommand(const Platform& platform, const path& library,
                        const std::vector<path>& objects) {
    std::vector<string> object_strings;
    for (const path& object : objects)
      object_strings.push_back(object.string());
    return EncodeAsString(platform.tool_prefix, "ar rcs ", library.string(),
                          " ", boost::algorithm::join(object_strings, " "));
  }

  string ProgramCommand(const Platform& platform, const path& program,
                        const std::set<path>& libraries) {
    std::vector<string> library_strings;
    for (const path& library : libraries)
      library_strings.push_back(library.string());
    return EncodeAsString(
        platform.tool_prefix, "g++ -std=gnu++14 -g -O3 -static -o ",
        program.string(), " -Wl,--start-group ",
        boost::algorithm::join(library_strings, " "),
        " -Wl,--end-group -lboost_filesystem -lboost_system "
        "-Wl,--whole-archive -lpthread -Wl,--no-whole-archive");
  }

  void Build(const std::vector<string>& args) {
    struct Platform zubu = {"zubu", "x86_64-zubu-linux-gnu-"};
    struct Platform zipi = {"zipi", "arm-zipi-linux-gnueabihf-"};

    static std::vector<Platform> platforms = {zubu, zipi};
    const path source_root = GetSourceRoot();
    const SourceTree source_tree = GetSourceTree();

    std::map<RuleRef, std::set<RuleRef>> ruledeps;

    RuleRef primitives_rule("", "primitives");

    for (const auto& directory_kv : source_tree) {
      const string& directory_name = directory_kv.first;
      const SourceDirectory& directory = directory_kv.second;

      for (const auto& rule_kv : directory.rules) {
        const string& rule_name = rule_kv.first;

        RuleRef me(directory_name, rule_name);
        std::set<RuleRef> deps;
        deps.insert(me);
        deps.insert(primitives_rule);
        bool deps_added;
        do {
          deps_added = false;
          std::set<RuleRef> deps_to_add;
          for (const RuleRef& inner : deps) {
            MUST(ContainsKey(source_tree, inner.directory) &&
                     ContainsKey(source_tree.at(inner.directory).rules,
                                 inner.name),
                 "bad rule ref: ", inner.directory, "/", inner.name);
            const Rule& inner_rule =
                source_tree.at(inner.directory).rules.at(inner.name);
            for (const RuleRef& outer : inner_rule.dependencies) {
              if (!Contains(deps, outer)) {
                deps_added = true;
                deps_to_add.insert(outer);
              }
            }
          }
          for (const RuleRef& dep_to_add : deps_to_add) {
            deps.insert(dep_to_add);
          }
        } while (deps_added);

        ruledeps[me] = deps;
      }
    }

    const path primitives_header = canonical(GetSourceRoot() / "primitives.h");

    const path headers_superroot = GetWheeDir() / "headers";
    remove_all(headers_superroot);
    const path units_superroot = GetWheeDir() / "units";
    remove_all(units_superroot);

    for (const auto& directory_kv : source_tree) {
      const string& directory_name = directory_kv.first;
      const SourceDirectory& directory = directory_kv.second;

      for (const auto& rule_kv : directory.rules) {
        const string& rule_name = rule_kv.first;
        const Rule& rule = rule_kv.second;

        const path headers_root =
            headers_superroot / directory_name / rule_name;
        const path units_root = units_superroot / directory_name / rule_name;

        for (const string& header : rule.headers) {
          const path origin_header = source_root / directory_name / header;
          const path linked_header = headers_root / directory_name / header;
          create_directories(headers_root / directory_name);
          create_hard_link(origin_header, linked_header);
        }

        for (const string& unit : rule.sources) {
          const path origin_unit = source_root / directory_name / unit;
          const path linked_unit = units_root / directory_name / unit;
          create_directories(units_root / directory_name);
          create_hard_link(origin_unit, linked_unit);
        }
      }
    }

    for (const Platform& platform : platforms) {
      const path build_root = GetWheeDir() / "build" / platform.name;
      remove_all(build_root);
      const path programs_root = GetWheeDir() / "programs" / platform.name;
      remove_all(programs_root);

      std::map<RuleRef, path> library_files;

      for (const auto& directory_kv : source_tree) {
        const string& directory_name = directory_kv.first;
        const SourceDirectory& directory = directory_kv.second;

        const path build_directory = build_root / directory_name;

        create_directories(build_directory);

        for (const auto& rule_kv : directory.rules) {
          const string& rule_name = rule_kv.first;
          const Rule& rule = rule_kv.second;

          RuleRef me(directory_name, rule_name);

          const path headers_root =
              headers_superroot / directory_name / rule_name;
          const path units_root = units_superroot / directory_name / rule_name;

          std::vector<path> include_paths;

          for (const RuleRef& dep : ruledeps.at(me)) {
            include_paths.push_back(headers_superroot / dep.directory /
                                    dep.name);
          }

          std::vector<path> objects;
          for (const string& source_file : rule.sources) {
            const path source = units_root / directory_name / source_file;
            const path object =
                build_directory / (source.stem().string() + ".o");
            ExecuteShellCommand(CompileCommand(
                platform, source, object, primitives_header, include_paths));
            objects.push_back(object);
          }

          if (!objects.empty()) {
            const path library = build_directory / (rule_name + ".a");
            ExecuteShellCommand(LibraryCommand(platform, library, objects));
            library_files[me] = library;
          }
        }
      }

      for (const auto& directory_kv : source_tree) {
        const string& directory_name = directory_kv.first;
        const SourceDirectory& directory = directory_kv.second;

        const path programs_directory = programs_root / directory_name;

        create_directories(programs_directory);

        for (const auto& rule_kv : directory.rules) {
          const string& rule_name = rule_kv.first;
          const Rule& rule = rule_kv.second;
          RuleRef me(directory_name, rule_name);

          if (rule.kind != Rule::PROGRAM && rule.kind != Rule::TEST) continue;

          const path program = programs_directory / rule_name;

          std::set<path> libs;
          for (const RuleRef& dep : ruledeps.at(me)) {
            auto* p = FindOrNull(library_files, dep);
            if (p) libs.insert(*p);
          }

          ExecuteShellCommand(ProgramCommand(platform, program, libs));
        }
      }
    }
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
